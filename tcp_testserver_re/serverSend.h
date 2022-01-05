
#ifndef SERVER_SEND_H__
#define SERVER_SEND_H__

#include "netservice.h"
#include <mutex>
#include <list>

class ClientPool;
class LoopThread;
class NetPacket;
class WinSocket;

class ServerSend : public NetService
{
private:
    std::weak_ptr<ClientPool> m_cliPool;
    std::unique_ptr<LoopThread> m_sendThread;
    std::list<std::unique_ptr<NetPacket>> m_packetList;

public:
    explicit ServerSend();
    ~ServerSend() override;

private:
    bool EmptySlot() const;
    bool SendPacket(std::unique_ptr<NetPacket> &&msg);
    bool SendLoop();
    bool OnInitialize() override;
    bool OnStarted() override;
    void OnDeinitialize() override;
    void OnStopped() override;

    void WhenJoinedNewConnect(std::weak_ptr<WinSocket> sock);

public:
    void Commit(std::unique_ptr<NetPacket> &&msg);
    void RegistClientPool(std::weak_ptr<ClientPool> cliPool);

private:
    std::mutex m_lock;
};

#endif

