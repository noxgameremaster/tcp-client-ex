
#ifndef EXAMPLE_SERVER_H__
#define EXAMPLE_SERVER_H__

#include "netservice.h"

class WinSocket;
class ServerAccept;
class ServerReceiveEx;
class BufferPopper;
class ServerTaskManager;
class NetPacket;
class ServerSend;

class ExampleServer : public NetService
{
private:
    std::shared_ptr<WinSocket> m_listenSocket;
    std::unique_ptr<ServerAccept> m_accept;
    std::unique_ptr<ServerReceiveEx> m_receive;
    std::unique_ptr<BufferPopper> m_servWorker;
    std::unique_ptr<ServerTaskManager> m_serverTaskManager;
    std::unique_ptr<ServerSend> m_servSend;

public:
    explicit ExampleServer();
    ~ExampleServer();

private:
    bool MakeServerSocket();
    bool OnInitialize() override;
    bool OnStarted() override;
    void OnDeinitialize() override;
    void OnStopped() override;

    void SlotForwardPacket(std::unique_ptr<NetPacket> &&packet);
    void SlotSendPacket(std::unique_ptr<NetPacket> &&packet);
};

#endif

