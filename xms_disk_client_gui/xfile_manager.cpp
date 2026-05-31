#include "xfile_manager.h"
#include "xtools.h"
using namespace std;
using namespace xdisk;
XFileManager *XFileManager::instance_ = 0;


xmsg::XServiceList XFileManager::upload_servers()
{
    XMUTEX(&servers_mutex_);
    return upload_servers_;
}
xmsg::XServiceList XFileManager::download_servers()
{
    XMUTEX(&servers_mutex_);
    return download_servers_;
}

void XFileManager::set_upload_servers(xmsg::XServiceList servers)
{
    XMUTEX(&servers_mutex_);
    this->upload_servers_ = servers;

}
void XFileManager::set_download_servers(xmsg::XServiceList servers)
{
    XMUTEX(&servers_mutex_);
    this->download_servers_ = servers;
}
int XFileManager::AddUploadTask(xdisk::XFileInfo file)
{
    XFileTask task;
    auto filetask = new XFileInfo();
    filetask->CopyFrom(file);
    //只能用动态分配的空间，会在task 引用计数为0清理时delete
    task.set_allocated_file(filetask);
    static int task_id = 0;
    task_id++;
    task.set_index(task_id);
    task.set_tasktime(XGetTime());

    {
        XMutex mutex(&uploads_mutex_);
        uploads_.push_back(task);
    }
    UploadProcess(task_id, 0);
    return task_id;
}
int XFileManager::AddDownloadTask(xdisk::XFileInfo file)
{
    XFileTask task;
    auto filetask = new XFileInfo();
    filetask->CopyFrom(file);
    //只能用动态分配的空间，会在task 引用计数为0清理时delete
    task.set_allocated_file(filetask);
    static int task_id = 0;
    task_id++;
    task.set_index(task_id);
    task.set_tasktime(XGetTime());

    {
        XMutex mutex(&downloads_mutex_);
        downloads_.push_back(task);
    }
    DownloadProcess(task_id, 0);
    return task_id;
}
void XFileManager::UploadEnd(int task_id)
{
    {
        XMutex mutex(&uploads_mutex_);
        for (auto up = uploads_.begin(); up != uploads_.end(); up++)
        {
            if (task_id == up->index())
            {
                up->set_is_complete(true);
                up->mutable_file()->set_net_size(up->file().filesize());
                break;
            }
        }
        RefreshUploadTask(uploads_);
    }
    // 锁已释放，通知传输完成计数更新
    emit RefreshCompletedTaskCount(GetCompletedTasks().size());
}

//进度从
void XFileManager::UploadProcess(int task_id, long long sended)
{
    XMutex mutex(&uploads_mutex_);
    for (auto up = uploads_.begin(); up != uploads_.end(); up++)
    {
        if (task_id == up->index())
        {
            up->mutable_file()->set_net_size(sended);
        }
    }
    RefreshUploadTask(uploads_);
}

void XFileManager::DownloadEnd(int task_id)
{
    {
        XMutex mutex(&downloads_mutex_);
        for (auto up = downloads_.begin(); up != downloads_.end(); up++)
        {
            if (task_id == up->index())
            {
                up->set_is_complete(true);
                up->mutable_file()->set_net_size(up->file().filesize());
                break;
            }
        }
        RefreshDownloadTask(downloads_);
    }
    emit RefreshCompletedTaskCount(GetCompletedTasks().size());
}

void XFileManager::DownloadProcess(int task_id, long long recved)
{
    XMutex mutex(&downloads_mutex_);
    for (auto down = downloads_.begin(); down != downloads_.end(); down++)
    {
        if (task_id == down->index())
        {
            down->mutable_file()->set_net_size(recved);
        }
    }
    RefreshDownloadTask(downloads_);
}

std::list<xdisk::XFileTask> XFileManager::GetCompletedTasks()
{
    std::list<xdisk::XFileTask> result;
    {
        XMutex mux(&uploads_mutex_);
        for (auto& t : uploads_)
        {
            if (t.is_complete())
            {
                result.push_back(t);
                result.back().set_is_sec(true);  // upload
            }
        }
    }
    {
        XMutex mux(&downloads_mutex_);
        for (auto& t : downloads_)
        {
            if (t.is_complete())
            {
                result.push_back(t);
                result.back().set_is_sec(false);  // download
            }
        }
    }
    return result;
}

