
#include "netclient.h"
#include "netflowcontrol.h"
#include "clientreceive.h"
#include "clientsend.h"
#include "winsocket.h"

NetClient::NetClient()
    : NetService()
{
    m_flowcontrol = std::make_unique<NetFlowControl>();
}

NetClient::~NetClient()
{ }

bool NetClient::StandBySocket()
{
    std::unique_ptr<WinSocket> sock(new WinSocket);

    if (!sock->CreateSocket())
        return false;
    if (!sock->Connect())
        return false;

    m_netsocket = decltype(m_netsocket)(sock.release());
    return true;
}

bool NetClient::ReceiverInit()
{
    decltype(m_receiver) receiver(new ClientReceive(m_netsocket, this));

    ShareOption(receiver.get());

    if (!receiver->Startup())
        return false;

    m_receiver = std::move(receiver);
    return true;
}

bool NetClient::SenderInit()
{
    decltype(m_sender) sender(new ClientSend(m_netsocket, this));

    if (!sender->Startup())
        return false;

    m_sender = std::move(sender);
    return true;
}

bool NetClient::OnStarted()
{
    auto checkException = [](bool cond)
    {
        if (!cond)
            throw false;
    };

    SetNetOption("127.0.0.1", 18590);

    try
    {
        checkException(StandBySocket());
        checkException(ReceiverInit());
        checkException(SenderInit());
    }
    catch (const bool &fail)
    {
        return fail;
    }

    return true;
}

void NetClient::OnStopped()
{
    if (m_receiver)
        m_receiver->Shutdown();
    if (m_sender)
        m_sender->Shutdown();
}

std::weak_ptr<NetFlowControl> NetClient::FlowControl()
{
    return m_flowcontrol;
}

