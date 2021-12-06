
#ifndef NET_CLIENT_H__
#define NET_CLIENT_H__

#include "netservice.h"

class ClientReceive;
class ClientSend;
class WinSocket;
class NetFlowControl;

class NetClient : public NetService
{
private:
    std::unique_ptr<ClientReceive> m_receiver;
    std::unique_ptr<ClientSend> m_sender;
    std::shared_ptr<WinSocket> m_netsocket;
    std::shared_ptr<NetFlowControl> m_flowcontrol;

public:
    explicit NetClient();
    ~NetClient() override;

private:
    bool StandBySocket();
    bool ReceiverInit();
    bool SenderInit();
    bool OnStarted() override;
    void OnStopped() override;

public:
    std::weak_ptr<NetFlowControl> FlowControl();
};

#endif

