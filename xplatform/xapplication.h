#ifndef XAPPLICATION_H
#define XAPPLICATION_H
#include <string>

class XApplication
{
public:    
    static std::string Application;         //应用名称
    static std::string ServerName;          //服务名称,一个服务名称含一个或多个服务标识
    static std::string BasePath;            //应用程序路径，用于保存远程系统配置的本地目录
    static std::string DataPath;            //应用程序数据路径用于保存普通数据文件
    static std::string LocalIp;             //本机IP
    static std::string LogPath;             //log路径
    static std::string LogLevel;            //log日志级别
    static std::string Local;               //本地套接字
    static std::string Node;                //本机node地址
    static std::string Log;                 //日志中心地址
    static std::string Config;              //配置中心地址
    static std::string Notify;              //信息通知中心
    static std::string ConfigFile;          //框架配置文件路径
    /**
     * 应用构造
     */
    XApplication();

    /**
     * 应用析构
     */
    ~XApplication();

    /**
     * 初始化
     * @param argv
     */
    void main(int argc, char *argv[]);
    
    /**
     * 运行
     */
    void WaitForShutdown();

protected:
    /**
    * 初始化, 只会进程调用一次
    */
    virtual void Initialize() = 0;

    /**
     * 析够, 进程只会调用一次
     */
    virtual void DestroyApp() = 0;

};

#endif