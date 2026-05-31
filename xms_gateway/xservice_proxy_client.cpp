#include "xservice_proxy_client.h"
#include "xtools.h"
#include "xauth_proxy.h"
#include "xlog_client.h"
using namespace std;

XServiceProxyClient* XServiceProxyClient::Create(std::string service_name)
{
    if (service_name == AUTH_NAME)
    {
        return new XAuthProxy();
    }
    return new XServiceProxyClient();
}

bool XServiceProxyClient::SendMsg(xmsg::XMsgHead *head, XMsg *msg, XMsgEvent *ev)
{
    RegEvent(ev);
    head->set_msg_id((long long)ev);
    return XMsgEvent::SendMsg(head, msg);
}
void XServiceProxyClient::DelEvent(XMsgEvent *ev)
{
    XMutex mux(&callback_task_mutex_);
    callback_task_.erase((long long)ev);
}
//注册一个事件
void XServiceProxyClient::RegEvent(XMsgEvent *ev)
{
    XMutex mux(&callback_task_mutex_);
    callback_task_[(long long)ev] = ev;
}
void XServiceProxyClient::ReadCB(xmsg::XMsgHead *head, XMsg *msg)
{
    if (!head || !msg)return;

    cout << "[PROXY_READCB] this=" << this << " server=" << server_ip() << ":" << server_port() << endl;
    cout << "[PROXY_READCB] msg_id=" << head->msg_id() << " msg_type=" << head->msg_type() << endl;
    cout << "[PROXY_READCB] callback_task_ size=" << callback_task_.size() << endl;

    // 打印所有 callback_task_ 中的 key 以便比对
    cout << "[PROXY_READCB] callback_task_ keys:";
    for (auto &kv : callback_task_)
        cout << " " << kv.first;
    cout << endl;

    auto router = callback_task_.find(head->msg_id());
    if (router == callback_task_.end())
    {
        cout << "[PROXY_READCB] callback_task_ FIND FAILED for msg_id=" << head->msg_id() << endl;
        return;
    }
    cout << "[PROXY_READCB] callback_task_ FIND OK, forwarding to router=" << router->second << endl;
    // 多线程问题？？ 通过 锁 解决
    router->second->SendMsg(head, msg);
    cout << "[PROXY_READCB] forwarded OK" << endl;
}

XServiceProxyClient::XServiceProxyClient()
{
}


XServiceProxyClient::~XServiceProxyClient()
{
}
