
#ifndef SERVER_RECEIVE_EX_H__
#define SERVER_RECEIVE_EX_H__

#include "netservice.h"

class LoopThread;
class WinSocket;
class SocketSet;
class ClientPool;
class PacketBuffer;

class ServerReceiveEx : public NetService
{
    static constexpr size_t reserve_recv_buffer = 1024;
private:
    std::unique_ptr<LoopThread> m_receiveThread;
    std::shared_ptr<ClientPool> m_cliPool;
    std::unique_ptr<SocketSet> m_socketSet;
    std::vector<uint8_t> m_recvBuffer;
    std::shared_ptr<PacketBuffer> m_packetBuffer;

public:
    explicit ServerReceiveEx();
    ~ServerReceiveEx() override;

private:
    void OnDisconnected(WinSocket *client);
    void ReceiveFromClient(WinSocket *client);
    bool ReceiveData();
    bool OnInitialize() override;
    bool OnStarted() override;
    void OnDeinitialize() override;
    void OnStopped() override;

private:
    void SlotNewUser(std::weak_ptr<WinSocket> client);

public:
    void RegistClientPool(std::shared_ptr<ClientPool> &cliPool);

    DECLARE_SIGNAL(OnShareBuffer, std::shared_ptr<PacketBuffer>)
    DECLARE_SIGNAL(OnReceiveData)

private:
    std::mutex m_lock;
};

#endif

