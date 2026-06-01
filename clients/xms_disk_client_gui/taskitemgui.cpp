#include "taskitemgui.h"
#include "xtools.h"
#include <string>
using namespace std;
#define FILE_MID_ICON_PATH ":/XMSDiskClientGui/Resources/img/FileType/Middle/"
TaskItemGUI::TaskItemGUI(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    
    //////去除原窗口边框
    //setWindowFlags(Qt::FramelessWindowHint);

    //////隐藏背景，用于圆角
    //setAttribute(Qt::WA_TranslucentBackground);
    //ui.filename->setAttribute(Qt::WA_TranslucentBackground);
    //ui.filesize->setAttribute(Qt::WA_TranslucentBackground);
    ////ui.filetype->setAttribute(Qt::WA_TranslucentBackground);
    //ui.filetime->setAttribute(Qt::WA_TranslucentBackground);
    
}
void TaskItemGUI::SetTask(xdisk::XFileTask task)
{
    auto file = task.file();

    //任务事件
    ui.filetime->setText(task.tasktime().c_str());

    //文件名
    QString filename = QString::fromUtf8(task.file().filename().c_str());
    ui.filename->setText(filename);

    //文件大小/进度
    if (task.is_complete())
    {
        //已完成：只显示文件大小，进度条显示"上传完成"或"下载完成"
        stringstream ss;
        ss << XGetSizeString(file.filesize());
        ui.filesize->setText(ss.str().c_str());
        ui.progressBar->setMinimum(0);
        ui.progressBar->setMaximum(100);
        ui.progressBar->setValue(100);
        if (task.is_sec())
            ui.progressBar->setFormat(QString::fromUtf8("上传完成"));
        else
            ui.progressBar->setFormat(QString::fromUtf8("下载完成"));
    }
    else
    {
        //传输中：显示 已传/总大小 + 进度百分比
        stringstream ss;
        if (file.filesize() == 0)
        {
            ss << "0B" << endl;
        }
        else
        {
            ss << XGetSizeString(file.net_size()) << "/" << XGetSizeString(file.filesize());
        }
        ui.filesize->setText(ss.str().c_str());

        ui.progressBar->setMinimum(0);
        ui.progressBar->setFormat("%p%");
        long long filesize = file.filesize();
        long long netsize = file.net_size();
        if (filesize > 1024 * 1024 * 10)
        {
            filesize /= 1000;
            netsize /= 1000;
        }
        ui.progressBar->setMaximum(filesize);
        ui.progressBar->setValue(netsize);
    }

    ///文件图标
    string iconpath = FILE_MID_ICON_PATH;
    iconpath += XGetIconFilename(task.file().filename(), file.is_dir());
    iconpath += "Type.png";
    QString sty = "background-color: rgba(0, 0, 0,0);\n";
    sty += "background-image: url(";
    sty += iconpath.c_str();
    sty += ");";
    ui.filetype->setStyleSheet(sty);


}

TaskItemGUI::~TaskItemGUI()
{
}
