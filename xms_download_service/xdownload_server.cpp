#include "xdownload_handle.h"
#include "xdownload_server.h"
#include "xlog_client.h"
#include "xregister_client.h"
#include "xtools.h"
#include <sstream>
using namespace std;
void XDownloadServer::main(int argc, char *argv[])
{
    XDownloadHandle::RegMsgCallback();
    LOGDEBUG("xms_download_service register_ip register_port service_port [service_ip]");

    int service_port = DOWNLOAD_PORT;
    int register_port = REGISTER_PORT;
    string register_ip = XGetHostByName(API_REGISTER_SERVER_NAME);
    string service_ip = "";  // empty = use connection source IP

    if (argc > 1)
        register_ip = argv[1];
    if (argc > 2)
        register_port = atoi(argv[2]);
    if (argc > 3)
        service_port = atoi(argv[3]);
    if (argc > 4)
        service_ip = argv[4];

    set_server_port(service_port);

    XRegisterClient::Get()->set_server_ip(register_ip.c_str());
    XRegisterClient::Get()->set_server_port(register_port);
    XRegisterClient::Get()->RegisterServer(DOWNLOAD_NAME, service_port,
        service_ip.empty() ? 0 : service_ip.c_str(), true);


    auto log = XLogClient::Get();
    log->set_service_name(DOWNLOAD_NAME);
    string logfile = DOWNLOAD_NAME;
    stringstream ss;
    ss << DOWNLOAD_NAME <<"_"<< service_port << ".log";
    log->set_local_file(ss.str());
    //log->StartLog();

}
XServiceHandle* XDownloadServer::CreateServiceHandle()
{
    return new XDownloadHandle();
}
XDownloadServer::XDownloadServer()
{
}


XDownloadServer::~XDownloadServer()
{
}
