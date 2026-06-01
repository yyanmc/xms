#include "xget_dir_client.h"
#include "xfile_manager.h"
#include <iostream>
#include "xlog_client.h"
using namespace std;
using namespace xdisk;
using namespace xmsg;
XGetDirClient::XGetDirClient()
{
    set_service_name(DIR_NAME);
    set_timer_ms(3000);
}

// 定时获取 获取上传和下载的服务器列表
void XGetDirClient::GetService()
{
    XMsgHead head;
    head.set_msg_type(MSG_GET_OUT_SERVICE_REQ);
    //SetHead(&head);

    XGetServiceReq req;
    req.set_name(UPLOAD_NAME);
    head.set_service_name(req.name());   
    LOGINFO(head.DebugString());
    SendMsg(&head, &req);

    req.set_name(DOWNLOAD_NAME);
    head.set_service_name(req.name());
    SendMsg(&head, &req);


}
//定时器获取上传和下载服务器列表
void XGetDirClient::TimerCB()
{
    //GetService();
    //ConnectedCB 发送消息时机可能太早（连接未就绪），此处确保目录和磁盘信息能加载到
    cout << "[DBG] TimerCB: has_loaded_dir_=" << has_loaded_dir_ << " is_connected=" << is_connected() << endl;
    if (!has_loaded_dir_ && is_connected())
    {
        cout << "[DBG] TimerCB: sending GetDirReq and GetDiskInfoReq" << endl;
        XGetDirReq req;
        req.set_root("/");
        GetDirReq(req);
        GetDiskInfoReq();
    }
}


//获取网盘空间使用情况
void XGetDirClient::GetDiskInfoReq()
{
    XMessageRes req;
    req.set_msg("GET");
    SendMsg((MsgType)GET_DISK_INFO_REQ, &req);
}


//获取网盘空间使用情况
void XGetDirClient::GetDiskInfoRes(xmsg::XMsgHead *head, XMsg *msg)
{
    cout << "[DBG] GetDiskInfoRes CALLED" << endl;
    XDiskInfo res;
    if (!res.ParseFromArray(msg->data, msg->size))
    {
        LOGINFO("XGetDirClient::GetDiskInfoRes failed! ParseFromArray error");
        return;
    }
    cout << "[DBG] GetDiskInfoRes parsed OK" << endl;
    cout << res.DebugString();
    XFileManager::Instance()->RefreshDiskInfo(res);
}

void XGetDirClient::GetServiceRes(xmsg::XMsgHead *head, XMsg *msg)
{
    XServiceList res;
    if (!res.ParseFromArray(msg->data, msg->size))
    {
        LOGINFO("XGetDirClient::GetServiceRes failed! ParseFromArray error");
        return;
    }
    //cout << res.DebugString();

    if(res.name() == UPLOAD_NAME)
        XFileManager::Instance()->set_upload_servers(res);
    else
        XFileManager::Instance()->set_download_servers(res);
}
void XGetDirClient::NewDirReq(std::string path)
{
    XGetDirReq req;
    req.set_root(path);
    //XMsgHead head;
    //head.set_msg_type((MsgType)NEW_DIR_REQ);
    ////需要考虑登录信息
    //head.set_token("");
    //head.set_username("root");
    //SendMsg(&head, &req);
    SendMsg((MsgType)NEW_DIR_REQ, &req);
}

void XGetDirClient::NewDirRes(xmsg::XMsgHead *head, XMsg *msg)
{
    cout << "NewDirRes" << endl;
    xdisk::XGetDirReq req;
    req.set_root(cur_dir_);
    //删除成功
    GetDirReq(req);
}

void XGetDirClient::GetDirReq(xdisk::XGetDirReq req)
{
    //XMsgHead head;
    //head.set_msg_type((MsgType)GET_DIR_REQ);
    ////需要考虑登录信息
    //head.set_token(login_.token());
    //head.set_username(login_.username());
    //head.set_rolename(login_.rolename());
    //head.set_service_name(DIR_NAME);
    //SendMsg(&head, &req);
    SendMsg((MsgType)GET_DIR_REQ, &req);
    cur_dir_ = req.root();
}
//void XGetDirClient::TimerCB()
//{
//    XGetDirReq req;
//    bool is_find = false;
//    //多个任务只取最后一次 最多一次取10个
//    while (FileManager::Get()->GetTask(xdisk::GET_DIR_REQ, &req))
//    {
//        is_find = true;
//    }
//    if (!is_find)
//    {
//        return;
//    }
//    
//    XMsgHead head;
//    head.set_msg_type((MsgType)GET_DIR_REQ);
//    //需要考虑登录信息
//    head.set_token("");
//    head.set_username("root");
//    SendMsg(&head, &req);
//}
void XGetDirClient::GetDirRes(xmsg::XMsgHead *head, XMsg *msg)
{
    cout << "[DBG] GetDirRes CALLED! msg_size=" << (msg ? msg->size : -1) << endl;
    if (head)
        cout << "[DBG] GetDirRes head: " << head->DebugString() << endl;
    XFileInfoList flist;
    if (!flist.ParseFromArray(msg->data, msg->size))
    {
        cout << "[DBG] GetDirRes ParseFromArray failed!" << endl;
        cout << "GetDirRes ParseFromArray failed!" << endl;
        return;
    }
    cout << "[DBG] GetDirRes parsed OK, files_count=" << flist.files_size() << endl;
    XFileManager::Instance()->RefreshData(flist, cur_dir_);
    has_loaded_dir_ = true;
    cout << "[DBG] GetDirRes: has_loaded_dir_ set to true, retries will stop" << endl;
    //获取上传下载服务器列表
    GetService();

    //刷新磁盘空间使用情况
    GetDiskInfoReq();
}

void XGetDirClient::ConnectedCB()
{
    // 连接成功后自动加载文件列表和磁盘容量信息
    cout << "[DBG] ConnectedCB fired! is_connected=" << is_connected() << endl;
    XGetDirReq req;
    req.set_root("/");
    GetDirReq(req);
    GetDiskInfoReq();
}

void XGetDirClient::DeleteFileReq(xdisk::XFileInfo file)
{
    //XMsgHead head;
    //head.set_msg_type((MsgType)DELETE_FILE_REQ);
    ////需要考虑登录信息
    //head.set_token("");
    //head.set_username("root");
    //SendMsg(&head, &file);

    SendMsg((MsgType)DELETE_FILE_REQ, &file);
}

void XGetDirClient::DeleteFileRes(xmsg::XMsgHead *head, XMsg *msg)
{
    cout << "DeleteFileRes" << endl;
    xdisk::XGetDirReq req;
    req.set_root(cur_dir_);
    //删除成功
    GetDirReq(req);
}
XGetDirClient::~XGetDirClient()
{
}
