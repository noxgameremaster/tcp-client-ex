
#ifndef CLIENT_RECEIVE_H__
#define CLIENT_RECEIVE_H__

#include "netservice.h"
#include <thread>
#include <vector>

class WinSocket;
class IOBuffer;
class SocketSet;
class ClientWorker;

class ClientReceive : public NetService
{
private:
    std::shared_ptr<WinSocket> m_netsocket;
    std::unique_ptr<SocketSet> m_readFds;
    std::shared_ptr<IOBuffer> m_receivebuffer;
    std::unique_ptr<ClientWorker> m_networker;
    std::thread m_recvThread;
    bool m_terminated;

public:
    explicit ClientReceive(std::shared_ptr<WinSocket> &sock, NetObject *parent = nullptr);
    ~ClientReceive() override;

private:
    bool ErrorDisconnected();
    bool ErrorBufferIsFull();
    bool Receiving();
    void DoTask();

    bool OnInitialize() override;
    void OnDeinitialize() override;
    bool OnStarted() override;
    void HaltReceiveThread();
    void OnStopped() override;


};

#endif

