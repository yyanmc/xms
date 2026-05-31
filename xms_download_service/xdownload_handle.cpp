#include "xdownload_handle.h"
#include "xtools.h"
#include "xlog_client.h"
using namespace xmsg;
using namespace xdisk;
using namespace std;
#ifdef _WIN32
#define DIR_ROOT "./server_root/"
#else
#define DIR_ROOT "/root/xms/"
#endif
#define FILE_INFO_NAME_PRE ".info_"
#define FILE_SLICE_BYTE 100000000 

XDownloadHandle::XDownloadHandle()
{
    //设定定时器用于获取传送进度
    set_timer_ms(100);
    slice_buf_ = new char[FILE_SLICE_BYTE];
};

XDownloadHandle::~XDownloadHandle()
{
    delete slice_buf_;
    slice_buf_ = NULL;
}
void XDownloadHandle::DownloadFileReq(xmsg::XMsgHead *head, XMsg *msg)
{
    //验证用户权限
    //接收到文件请求
    if (!file_.ParseFromArray(msg->data, msg->size))
    {
        LOGDEBUG("UploadFileReq ParseFromArray failed!");
        return;
    }

    //DOWNLOAD_FILE_RES
    //容错没有信息的情况，有客户端判断文件是否有效

    
    XMessageRes res;
    string path = DIR_ROOT;
    path += head->username();
    path += "/";
    path += file_.filedir();
    path += "/";
    string filedir = path;
    path += file_.filename();

    string info_file = filedir;

    info_file += FILE_INFO_NAME_PRE;
    info_file += file_.filename();
    XFileInfo re_file;
    ifstream ifs(U8Path(info_file));
    if (!ifs || !re_file.ParseFromIstream(&ifs))
    {
        LOGINFO("file info read failed");
        re_file.CopyFrom(file_);
    }
    ifs.close();


    head->set_msg_type((MsgType)DOWNLOAD_FILE_RES);
    ifs_.open(U8Path(path), ios::binary);
    ifs_.seekg(0, ios::end);
    if (!ifs_)
    {
        //失败返回文件大小为0
        LOGINFO("DownloadFileReq: data file not found!");
        re_file.set_filesize(0);
        SendMsg(head, &re_file);
        return;
    }
    long long actual_size = ifs_.tellg();
    re_file.set_filesize(actual_size);
    file_.set_filesize(actual_size);
    ifs_.seekg(0,ios::beg);
    LOGINFO(("DownloadFileReq: file found, size=" + std::to_string(re_file.filesize())).c_str());
    SendMsg(head, &re_file);

    

    ////需要校验权限
    //res.set_return_(XMessageRes::OK);
    //res.set_msg("OK");
    //if (!ifs_)
    //{
    //    stringstream ss;
    //    ss << "UploadFileReq open file failed!" << path;
    //    res.set_return_(XMessageRes::ERROR);

    //    res.set_msg(ss.str());
    //    LOGINFO(ss.str().c_str())
    //}

    ////目录修改位实际目录 需要添加公共配置 上传目录

    ////res.set_return_(XMessageRes::ERROR);

    //head->set_msg_type((MsgType)UPLOAD_FILE_RES);
    //SendMsg(head, &res);
}

void XDownloadHandle::SendSlice()
{
    LOGINFO(("SendSlice: sendsize_=" + std::to_string(sendsize_) + " filesize_=" + std::to_string(file_.filesize()) + " eof=" + (ifs_.eof()?"1":"0") + " good=" + (ifs_.good()?"1":"0")).c_str());
    //文件已读完，发送空包通知客户端传输完成
    if (ifs_.eof() || sendsize_ >= file_.filesize())
    {
        XMsgHead head;
        head.set_msg_type((MsgType)DOWNLOAD_SLICE_REQ);
        XMsg data;
        data.data = nullptr;
        data.size = 0;
        SendMsg(&head, &data);
        LOGINFO("SendSlice: transfer complete");
        return;
    }

    ifs_.read(slice_buf_, FILE_SLICE_BYTE);
    int size = ifs_.gcount();
    if (size <= 0)
    {
        // 没有读到数据，EOF
        XMsgHead head;
        head.set_msg_type((MsgType)DOWNLOAD_SLICE_REQ);
        XMsg data;
        data.data = nullptr;
        data.size = 0;
        SendMsg(&head, &data);
        LOGINFO("SendSlice: transfer complete (no data read)");
        return;
    }

    sendsize_ += size;
    XMsgHead head;
    head.set_msg_type((MsgType)DOWNLOAD_SLICE_REQ);
    XMsg data;
    data.data = slice_buf_;
    data.size = size;
    SendMsg(&head, &data);
}
void XDownloadHandle::DownloadSliceRes(xmsg::XMsgHead *head, XMsg *msg)
{
    //校验md5
    //ifs_.read.write(msg->data, msg->size);

    //head->set_msg_type((MsgType)SEND_SLICE_RES);
    //XMessageRes res;
    ////需要校验权限
    //res.set_return_(XMessageRes::OK);
    //res.set_msg("OK");
    //SendMsg(head, &res);
    SendSlice();
}

void XDownloadHandle::DownloadFileBegin(xmsg::XMsgHead *head, XMsg *msg)
{
    SendSlice();
    //ofs_.close();
    ////验证文件md5 验证是否正确
    //head->set_msg_type((MsgType)UPLOAD_FILE_END_RES);
    //XMessageRes res;
    ////需要校验权限
    //res.set_return_(XMessageRes::OK);
    //res.set_msg("OK");
    //SendMsg(head, &res);
}