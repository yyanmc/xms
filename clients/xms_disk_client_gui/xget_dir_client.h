#pragma once
#include "xservice_client.h"
#include "xms_disk_client_gui.pb.h"

//定时从任务列表获取任务
// 需要考虑登录，是否放在XServiceClient中
// 要加入鉴权的内容

class XGetDirClient:public XServiceClient
{
public:
    static XGetDirClient* Get()
    {
        static XGetDirClient xc;
        return &xc;
    }
    //void TimerCB();
    XGetDirClient();

    ~XGetDirClient();

    void GetDirReq(xdisk::XGetDirReq req);
    void DeleteFileReq(xdisk::XFileInfo file);



    void GetDirRes(xmsg::XMsgHead *head, XMsg *msg);
    void DeleteFileRes(xmsg::XMsgHead *head, XMsg *msg);

    void NewDirReq(std::string path);
    
    void NewDirRes(xmsg::XMsgHead *head, XMsg *msg);


    //获取上传和下载的服务器列表
    void GetService() ;

    void GetServiceRes(xmsg::XMsgHead *head, XMsg *msg) ;


    //获取网盘空间使用情况
    void GetDiskInfoReq();

    //获取网盘空间使用情况
    void GetDiskInfoRes(xmsg::XMsgHead *head, XMsg *msg);

    //定时器获取上传和下载服务器列表
    virtual void TimerCB() override;


    //连接成功后自动发送获取目录和磁盘信息请求
    virtual void ConnectedCB() override;

    static void RegMsgCallback()
    {
        RegCB((xmsg::MsgType)xdisk::GET_DIR_RES, (MsgCBFunc)&XGetDirClient::GetDirRes);
        RegCB((xmsg::MsgType)xdisk::DELETE_FILE_RES, (MsgCBFunc)&XGetDirClient::DeleteFileRes);
        RegCB((xmsg::MsgType)xdisk::NEW_DIR_RES, (MsgCBFunc)&XGetDirClient::NewDirRes);
        RegCB((xmsg::MsgType)xdisk::GET_DISK_INFO_RES, (MsgCBFunc)&XGetDirClient::GetDiskInfoRes);
        RegCB(xmsg::MSG_GET_OUT_SERVICE_RES, (MsgCBFunc)&XGetDirClient::GetServiceRes);

        //XServiceList
    }
    std::string cur_dir() { return cur_dir_; }

    //void set_login(const xmsg::XLoginRes &login) { this->login_ = login; }

private:
    std::string cur_dir_ = "";
    bool has_loaded_dir_ = false; //TimerCB重试标记，加载成功后设为true

    //登录信息
    //xmsg::XLoginRes login_;


};

