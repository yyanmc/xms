// xms_dir_service.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include "xdir_service.h"
#include "xdir_handle.h"
#include "xregister_client.h"
#include "xtools.h"
#include <string>
using namespace std;
int main(int argc,char *argv[])
{
    std::cout << "xms_dir_service!\n"; 
    string register_ip = XGetHostByName(API_REGISTER_SERVER_NAME);
    XRegisterClient::Get()->set_server_ip(register_ip.c_str());

    XRegisterClient::Get()->RegisterServer(DIR_NAME, DIR_PORT, 0);
    XDirHandle::RegMsgCallback();
    XDirService service;
    service.set_server_port(DIR_PORT);
    service.Start();
    XThreadPool::Wait();
    return 0;
}
