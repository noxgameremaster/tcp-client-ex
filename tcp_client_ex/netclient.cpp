
#include "netclient.h"
#include "netflowcontrol.h"
#include "clientreceive.h"
#include "clientsend.h"
#include "winsocket.h"
#include "eventworker.h"

#include <iostream>

NetClient::NetClient()
    : NetService()
{
    m_flowcontrol = std::make_unique<NetFlowControl>();
}

NetClient::~NetClient()
{ }

void NetClient::OnDeinitialize()
{
    NetService::OnDeinitialize();

    ToggleEventManager(false);
}

void NetClient::ToggleEventManager(bool isOn)
{
    EventWorker &worker = EventWorker::Instance();

    isOn ? worker.Start() : worker.Stop();
}

void NetClient::OnError(const std::string &title, const std::string &errorMessage)
{
    if ("serverError" == title)
        std::cout << errorMessage << std::endl;
}

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

bool NetClient::OnInitialize()
{
    ToggleEventManager(true);

    NetService::OnInitialize();
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
    m_sender->SharedSendBuffer(m_flowcontrol.get(), &NetFlowControl::SetSendBuffer);
    return true;
}

bool NetClient::OnStarted()
{
    m_flowcontrol->Startup();
    return true;
}

void NetClient::OnStopped()
{
    if (m_receiver)
        m_receiver->Shutdown();
    if (m_sender)
        m_sender->Shutdown();
    if (m_flowcontrol)
        m_flowcontrol->Shutdown();
}

void NetClient::SlotReceivePacket(std::unique_ptr<NetPacket> &&packet)
{
    if (m_flowcontrol)
        m_flowcontrol->Enqueue(std::move(packet), NetFlowControl::IOType::IN);
}
