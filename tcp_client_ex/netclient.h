
#ifndef NET_CLIENT_H__
#define NET_CLIENT_H__

#include "netservice.h"

class ClientReceive;
class ClientSend;
class WinSocket;
class NetFlowControl;
class NetPacket;
class NetStatus;

class NetClient : public NetService
{
private:
    std::unique_ptr<ClientReceive> m_receiver;
    std::unique_ptr<ClientSend> m_sender;
    std::shared_ptr<WinSocket> m_netsocket;
    std::shared_ptr<NetFlowControl> m_flowcontrol;

    std::unique_ptr<NetStatus> m_netStatus;
    bool m_connected;

public:
    explicit NetClient();
    ~NetClient() override;

private:
    void OnInitialOnce() override;
    void OnError(const std::string &title, const std::string &errorMessage) override;
    bool StandBySocket();
    bool ReceiverInit();
    bool SenderInit();
    bool OnInitialize() override;
    bool OnStarted() override;
    void OnStopped() override;

    bool NetDebugInit();

public:
    void SlotReceivePacket(std::unique_ptr<NetPacket> &&packet);
    void ClientSendEcho();
    void ClientTestSendFileRequest(const std::string &req);
    void ClientSendChat(const std::string &say);
    bool SetNetworkParam(const std::string &ip, const std::string &port);
    void RegistInnerPacketListener(NetService *listener, std::function<void(std::shared_ptr<NetPacket>&&)> &&invokable);

private:
    void SlotReportPing(uint32_t recvCount, uint32_t sendCount);

    DECLARE_SIGNAL(OnDeadlockTest)
};

#endif

