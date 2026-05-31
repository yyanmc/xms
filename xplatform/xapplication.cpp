#include "xapplication.h"
#include "xthread_pool.h"

std::string XApplication::Application;      //应用名称
std::string XApplication::ServerName;       //服务名称,一个服务名称含一个或多个服务标识
std::string XApplication::LocalIp;          //本机IP
std::string XApplication::BasePath;         //应用程序路径，用于保存远程系统配置的本地目录
std::string XApplication::DataPath;         //应用程序路径，用于本地数据
std::string XApplication::Local;            //本地套接字
std::string XApplication::Node;             //本机node地址
std::string XApplication::Log;              //日志中心地址
std::string XApplication::Config;           //配置中心地址
std::string XApplication::Notify;           //信息通知中心
std::string XApplication::LogPath;          //logpath
std::string XApplication::LogLevel;            //log日志级别
std::string XApplication::ConfigFile;       //框架配置文件路径
/**
* 初始化
* @param argv
*/
void XApplication::main(int argc, char *argv[])
{
    //注册中心

}

/**
 * 运行
 */
void XApplication::WaitForShutdown()
{
    XThreadPool::Wait();
}

XApplication::XApplication()
{
}


XApplication::~XApplication()
{
}
