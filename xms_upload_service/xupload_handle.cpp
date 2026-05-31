#include "xupload_handle.h"
#include "xtools.h"
#include "xlog_client.h"
using namespace xmsg;
using namespace xdisk;
using namespace std;
#ifdef _WIN32
#define DIR_ROOT "./server_root/"
#else
#define DIR_ROOT "/mnt/xms/"
#endif
void XUploadHandle::UploadFileReq(xmsg::XMsgHead *head, XMsg *msg)
{
    //验证用户权限
    //接收到文件请求
    if (!cur_file_.ParseFromArray(msg->data, msg->size))
    {
        LOGDEBUG("UploadFileReq ParseFromArray failed!");
        return;
    }

    XMessageRes res;
    string path = DIR_ROOT;
    path += head->username();
    path += "/";
    path += cur_file_.filedir();
    save_dir_ = path;
    path += "/";
    cout << "UploadFileReq path = " << path << endl;
    //创建目录
    XNewDir(path);

    path += cur_file_.filename();

    ofs_.open(U8Path(path), ios::binary);

    //需要校验权限
    res.set_return_(XMessageRes::OK);
    res.set_msg("OK");
    if (!ofs_.is_open())
    {
        stringstream ss;
        ss << "UploadFileReq open file failed!" << path;
        res.set_return_(XMessageRes::ERROR);

        res.set_msg(ss.str());
        LOGINFO(ss.str().c_str())
    }

    //目录修改位实际目录 需要添加公共配置 上传目录

    //res.set_return_(XMessageRes::ERROR);

    head->set_msg_type((MsgType)UPLOAD_FILE_RES);
    SendMsg(head, &res);
}

void XUploadHandle::SendSliceReq(xmsg::XMsgHead *head, XMsg *msg)
{
    head->set_msg_type((MsgType)SEND_SLICE_RES);
    XMessageRes res;
    if (head->md5().empty())
    {
        cout << "XUploadHandle::SendSliceReq failed! md5 is empty!";

        //需要校验权限
        res.set_return_(XMessageRes::ERROR);
        res.set_msg("md5 is empty");
        SendMsg(head, &res);
        return;
    }
    //校验md5
    string md5 = XMD5_base64((unsigned char *)msg->data, msg->size);
    if (head->md5() != md5)
    {
        cout << "XUploadHandle::SendSliceReq failed! md5 is error!";
        res.set_return_(XMessageRes::ERROR);
        res.set_msg("md5 is error");
        SendMsg(head, &res);
        return;
    }

    ofs_.write(msg->data, msg->size);

    //需要校验权限
    res.set_return_(XMessageRes::OK);
    res.set_msg("OK");
    SendMsg(head, &res);
}

void XUploadHandle::UploadFileEndReq(xmsg::XMsgHead *head, XMsg *msg)
{
    ofs_.close();
    //验证文件md5 验证是否正确
    head->set_msg_type((MsgType)UPLOAD_FILE_END_RES);
    XMessageRes res;
    //需要校验权限
    res.set_return_(XMessageRes::OK);
    res.set_msg("OK");
    SendMsg(head, &res);
}