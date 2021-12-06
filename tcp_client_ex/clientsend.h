
#ifndef CLIENT_SEND_H__
#define CLIENT_SEND_H__

#include "netservice.h"
#include <thread>
#include <mutex>

class WinSocket;
class IOBuffer;

class ClientSend : public NetService
{
private:
    bool m_terminated;
    std::condition_variable m_condvar;
    std::shared_ptr<WinSocket> m_netsocket;
    std::shared_ptr<IOBuffer> m_sendbuffer;
    std::thread m_sendThread;

public:
    ClientSend(std::shared_ptr<WinSocket> &sock, NetObject *parent = nullptr);
    ~ClientSend() override;

private:
    void BufferOnPushed();
    void StreamSend();
    void DoTask();
    void HaltSendThread();
    bool OnInitialize() override;
    void OnDeinitialize() override;
    bool OnStarted() override;
    void OnStopped() override;

private:
    std::mutex m_waitLock;
};

#endif

