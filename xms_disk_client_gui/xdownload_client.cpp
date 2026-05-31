#include "xdownload_client.h"
#include"xmsg_com.pb.h"
#include "xms_disk_client_gui.pb.h"
#include "xfile_manager.h"
#include "xtools.h"

using namespace std;
using namespace xmsg;
using namespace xdisk;

bool XDownloadClient::set_file(xdisk::XFileInfo file)
{
    this->file_ = file;
    ofs_.open(U8Path(file.local_path()), ios::binary);
    if (!ofs_.is_open())
    {
        cout << "set_file " << file.local_path() << endl;
        return false;
    }
    return true;
}

void XDownloadClient::ConnectedCB()
{
    //XMsgHead head;
    //head.set_msg_type((MsgType)DOWNLOAD_FILE_REQ);
    //head.set_username("root");// 临时测试用，后面改为登陆信息
    //SendMsg(&head, &file_);
    SendMsg((MsgType)DOWNLOAD_FILE_REQ, &file_);
    cout << "XDownloadClient::Connect()" << endl;
}

//确认文件信息
void XDownloadClient::DownloadFileRes(xmsg::XMsgHead *head, XMsg *msg)
{/*
    XFileInfo res;*/
    //保存客户端本地路径和密码，服务器响应中不包含这些字段
    string local_path = file_.local_path();
    string password = file_.password();
    if (!file_.ParseFromArray(msg->data, msg->size))
    {
        cout << "XDownloadClient::DownloadFileRes ParseFromArray failed!" << endl;
        return;
    }
    file_.set_local_path(local_path);
    file_.set_password(password);
    //文件加密，需要有秘钥
    if (file_.is_enc())
    {
        auto pass = XFileManager::Instance()->password();
        if (pass.empty())
        {
            LOGERROR("please set password");
            //具体的提示的语言，可以根据字符串替换为不同的语言
            XFileManager::Instance()->ErrorSig("NO PASSWORD");
            return;
        }
        aes_ = XAES::Create();
        aes_->SetKey(pass.c_str(),pass.size(),false);
    }

    //如果是加密文件需要验证加密
    int task_id = XFileManager::Instance()->AddDownloadTask(file_);
    task_id_ = task_id;

    //XMsgHead h;
    //h.set_msg_type((MsgType)DOWNLOAD_FILE_BEGTIN);
    //h.set_username("root");// 临时测试用，后面改为登陆信息
    //SendMsg(&h, &file_);
    SendMsg((MsgType)DOWNLOAD_FILE_BEGTIN, &file_);

    begin_recv_data_size_ = recv_data_size();
    XFileManager::Instance()->DownloadProcess(task_id, 0);

}

void XDownloadClient::DownloadSliceReq(xmsg::XMsgHead *head, XMsg *msg)
{
    // EOF：服务端发送空包表示传输完成
    if (!msg || !msg->data || msg->size <= 0)
    {
        XFileManager::Instance()->DownloadEnd(task_id_);
        ofs_.close();
        ClearTimer();
        Close();
        return;
    }

    long long recved = file_.net_size() + msg->size;
    file_.set_net_size(recved);
    const char *data = msg->data;
    int size = msg->size;
    bool is_encrypted = false;
    if (file_.is_enc())
    {
        char *dec_data = new char[msg->size];
        size = aes_->Decrypt((unsigned char*)msg->data, msg->size, (unsigned char*)dec_data);
        if (size <= 0)
        {
            LOGERROR("aes_->Decrypt failed!");
            delete[] dec_data;
            return;
        }
        if (recved > file_.ori_size())
        {
            size = size - (recved - file_.ori_size());
        }
        data = dec_data;
        is_encrypted = true;
    }

    string md5_base64 = XMD5_base64((unsigned char*)data, size);
    // md5_base64s_.push_back(md5_base64);
    all_md5_base64_ += md5_base64;

    ofs_.write(data, size);
    if (is_encrypted)
    {
        delete[] data;
    }
    //XMsgHead h;
    //h.set_msg_type((MsgType)DOWNLOAD_SLICE_RES);
    //h.set_username("root");// 临时测试用，后面改为登陆信息
    //SendMsg(&h, &file_);
    SendMsg((MsgType)DOWNLOAD_SLICE_RES, &file_);
    //文件接收结束
    if (file_.filesize() <= file_.net_size())
    {
        XFileManager::Instance()->DownloadEnd(task_id_);
        //校验整个文件的md5
        ofs_.close();

        string file_md5 = XMD5_base64((unsigned char*)all_md5_base64_.data(), all_md5_base64_.size());
        if (file_.md5() != file_md5)
        {
            cerr << "file is not complete" << endl;
        }

        ClearTimer();
        Close();
    }
}
//通过定时器跟踪进度
void XDownloadClient::TimerCB()
{
    if (begin_recv_data_size_ < 0)
        return;
    auto size = BufferSize();


    //已发送的数据
    long long recved = recv_data_size() - begin_recv_data_size_ ;

    cout << recved << ":" << file_.filesize() << endl;
    XFileManager::Instance()->DownloadProcess(task_id_, recved);
}

XDownloadClient::XDownloadClient()
{
    set_timer_ms(100);
}


XDownloadClient::~XDownloadClient()
{
    if (aes_)
    {
        aes_->Drop();
        aes_ = NULL;
    }
}
