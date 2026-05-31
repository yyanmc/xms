// xms_upload_service.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "xlog_client.h"
#include "xdownload_server.h"
#include <iostream>

using namespace std;
int main(int argc,char *argv[])
{
    XDownloadServer server;
    server.main(argc,argv);
    server.Start();
    std::cout << DOWNLOAD_NAME << endl;
    server.Wait();
    return 0;
}
