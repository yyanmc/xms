#pragma once
#include "xservice_client.h"
#include "xms_disk_client_gui.pb.h"
#include <fstream>
#include <list>
#include "xtools.h"
//每个文件创建一个upload对象
class XUploadClient :public XServiceClient
{
public:
    XUploadClient();
    ~XUploadClient();
    virtual void ConnectedCB() override;
    void UploadFileRes(xmsg::XMsgHead *head, XMsg *msg);
    void UploadFileEndRes(xmsg::XMsgHead *head, XMsg *msg);
    void SendSliceRes(xmsg::XMsgHead *head, XMsg *msg);
    
    static void RegMsgCallback()
    {
        RegCB((xmsg::MsgType)xdisk::UPLOAD_FILE_RES, (MsgCBFunc)&XUploadClient::UploadFileRes);
        RegCB((xmsg::MsgType)xdisk::UPLOAD_FILE_END_RES, (MsgCBFunc)&XUploadClient::UploadFileEndRes);
        RegCB((xmsg::MsgType)xdisk::SEND_SLICE_RES, (MsgCBFunc)&XUploadClient::SendSliceRes);
    }

    bool set_file(xdisk::XFileInfo file);

    //设定文件的加密的 线程安全
    //void set_password(std::string password);
    //std::string password();


    void TimerCB() override;

    int task_id = 0;
private:

    //读取文件 并发送文件片
    void SendSlice();

    //上传文件信息
    xdisk::XFileInfo file_;

    //上传的文件
    std::ifstream ifs_;

    //读取文件片是用的缓存
    char *slice_buf_ = 0;

    //读取文件片是用的缓存(加密后)
    char *slice_buf_enc_ = 0;

    //开始发送数据时，已经发送的值，要确保缓冲已经都发送成功
    long long begin_send_data_size_ = -1;

    std::list<std::string> md5_base64s_;

    std::string password_;
    std::mutex  password_mutex_;

    //加密文件用
    XAES *aes_ = 0;

};

