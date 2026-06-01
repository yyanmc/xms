#include "login_gui..h"
#include <QMessageBox>
#include <string>
#include <sstream>
#include "xtools.h"
#include "xconfig_client.h"
#include <QTime>
#include "config_edit.h"
#include <QMessageBox>
#include "xconfig_manager.h"
#include "xauth_client.h"
#include "xmsg_com.pb.h"
#include "login_gui..h"
using namespace xmsg;
using namespace std;
void LoginGUI::Login()
{
    if (ui.usernameEdit->text().isEmpty())
    {
        QMessageBox::information(this, "", QString::fromUtf8("用户名不能为空"));
        return;
    }
    username_ = ui.usernameEdit->text().toStdString();
    if (ui.passwordEdit->text().isEmpty())
    {
        QMessageBox::information(this, "", QString::fromUtf8("密码不能为空"));
        return;
    }
    password_ = ui.passwordEdit->text().toStdString();



    XLoginRes login;
    for (int retry = 0; retry < 3; retry++)
    {
        XAUTH->LoginReq(username_, password_);
        this_thread::sleep_for(500ms);
        if (XAuthClient::Get()->GetLoginInfo(username_, &login, 5000))
        {
            cout << "login success!" << endl;
            MCONF->set_login(login);
            accept();
            return;
        }
        cout << "login retry " << retry + 1 << endl;
    }
    QMessageBox::information(this, "", QString::fromUtf8("用户名或者密码错误"));
}
LoginGUI::LoginGUI(QDialog *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
}

LoginGUI::~LoginGUI()
{
}
