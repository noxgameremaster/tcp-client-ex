
#ifndef CLIENT_SEND_H__
#define CLIENT_SEND_H__

#include "netservice.h"

class WinSocket;
class IOBuffer;
class LoopThread;

class ClientSend : public NetService
{
private:
    std::shared_ptr<WinSocket> m_netsocket;
    std::shared_ptr<IOBuffer> m_sendbuffer;
    std::unique_ptr<LoopThread> m_sendThread;

public:
    ClientSend(std::shared_ptr<WinSocket> &sock, NetObject *parent = nullptr);
    ~ClientSend() override;

private:
    void BufferOnPushed();
    bool StreamSend();
    bool OnInitialize() override;
    void OnDeinitialize() override;
    bool OnStarted() override;
    void OnStopped() override;

    std::string ObjectName() const override
    {
        return "ClientSend";
    }

public:
    template <class NetObjectPtr, class Function>
    void SharedSendBuffer(NetObjectPtr *obj, Function &&memberF)
    {
        if (nullptr == obj)
            return;

        static_assert(std::is_base_of<NetObject, NetObjectPtr>::value, "the instance must be inherit NetObject");

        (obj->*memberF)(m_sendbuffer);
    }
};

#endif

