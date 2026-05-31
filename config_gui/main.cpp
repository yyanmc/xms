#include "config_gui.h"
#include "xconfig_client.h"
#include "config_edit.h"
#include <QtWidgets/QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    //初始化化配置客户端，创建线程池
   //XConfigClient::Get()->StartGetConf("127.0.0.1", CONFIG_PORT, 0, 0, 0);
    QApplication a(argc, argv);
    // 设置支持中文的默认字体，解决中文乱码问题
    QFont font = a.font();
    font.setFamily(QString::fromUtf8("Microsoft YaHei"));
    font.setPointSize(10);
    a.setFont(font);
    //ConfigEdit edit;
    //edit.exec();
   // return 0;
    ConfigGui w;
    w.show();
    return a.exec();
}
