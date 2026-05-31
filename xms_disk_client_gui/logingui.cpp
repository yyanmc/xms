#include "logingui.h"
#include <QMouseEvent>
#include "xauth_client.h"
#include <string>
using namespace std;
LoginGUI::LoginGUI(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    ////去除原窗口边框
    setWindowFlags(Qt::FramelessWindowHint);

    ////隐藏背景，用于圆角
    setAttribute(Qt::WA_TranslucentBackground);
    ui.err_frame->hide();


}

LoginGUI::~LoginGUI()
{
}

void LoginGUI::Login()
{
    ui.err_frame->show();
    string username = ui.usernameEdit->text().toStdString();
    string password = ui.passwordEdit->text().toStdString();
    if (username.empty() || password.empty())
    {
        ui.err_msg->setText(QString::fromUtf8("用户名和密码不能为空！"));
        return;
    }

    if (!XAuthClient::Get()->Login(username, password))
    {

        ui.err_msg->setText(QString::fromUtf8("用户名或者密码有误！"));
        return;
    }
    static int count = 0;
    count++;
    
    ui.err_msg->setText(QString::number(count)+QString::fromUtf8("登录成功！"));
    QDialog::accept();
}


static bool mouse_press = false;
static QPoint mouse_point;
void LoginGUI::mouseMoveEvent(QMouseEvent *ev)
{
    //没有按下，处理原事件
    if (!mouse_press)
    {
        QWidget::mouseMoveEvent(ev);
        return;
    }
    auto cur_pos = ev->globalPos();
    this->move(cur_pos - mouse_point);
}
void LoginGUI::mousePressEvent(QMouseEvent *ev)
{
    //鼠标左键按下记录位置
    if (ev->button() == Qt::LeftButton)
    {
        mouse_press = true;
        mouse_point = ev->pos();
    }

}
void LoginGUI::mouseReleaseEvent(QMouseEvent *ev)
{
    mouse_press = false;
}