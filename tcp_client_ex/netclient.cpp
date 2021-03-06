
#include "netclient.h"
#include "netflowcontrol.h"
#include "clientreceive.h"
#include "clientsend.h"
#include "netStatus.h"
#include "winsocket.h"
#include "eventworker.h"
#include "stringHelper.h"
#include "netLogObject.h"

#include <iostream>
#include <cassert>
#include <sstream>

using namespace _StringHelper;

NetClient::NetClient()
    : NetService()
{
    m_flowcontrol = std::make_unique<NetFlowControl>();
    m_receiver = std::make_unique<ClientReceive>(this);
    m_sender = std::make_unique<ClientSend>(this);

    m_connected = false;
}

NetClient::~NetClient()
{ }

void NetClient::OnInitialOnce()
{
    m_sender->SharedSendBuffer(m_flowcontrol.get(), &NetFlowControl::SetSendBuffer);
}

void NetClient::OnError(const std::string &title, const std::string &errorMessage)
{
    if ("serverError" == title)
        NET_PUSH_LOGMSG(errorMessage, PrintUtil::ConsoleColor::COLOR_RED);
}

bool NetClient::StandBySocket()
{
    std::unique_ptr<WinSocket> sock(new WinSocket);

    ShareOption(sock.get());
    if (!sock->CreateSocket())
        return false;
    if (!sock->Connect())
        return false;

    m_netsocket = decltype(m_netsocket)(sock.release());
    return true;
}

bool NetClient::ReceiverInit()
{
    ShareOption(m_receiver.get());
    m_receiver->SetReceiveSocket(m_netsocket);

    return m_receiver->Startup();
}

bool NetClient::SenderInit()
{
    m_sender->SetSendSocket(m_netsocket);

    return m_sender->Startup();
}

bool NetClient::OnInitialize()
{
    NetService::OnInitialize();

    auto checkException = [](bool cond)
    {
        if (!cond)
            throw false;
    };

    try
    {
        checkException(StandBySocket());
        checkException(NetDebugInit());
        checkException(ReceiverInit());
        checkException(SenderInit());
    }
    catch (const bool &fail)
    {
        return fail;
    }

    return true;
}

bool NetClient::OnStarted()
{
    bool end = m_flowcontrol->Startup();

    NET_PUSH_LOGMSG(stringFormat("netclient::onstarted::show_result::%s", end ? "ok" : "ng"),
        end ? PrintUtil::ConsoleColor::COLOR_BLUE : PrintUtil::ConsoleColor::COLOR_RED);
    m_netStatus->Startup();
    return end;
}

void NetClient::OnStopped()
{
    if (m_receiver)
        m_receiver->Shutdown();
    if (m_sender)
        m_sender->Shutdown();
    if (m_flowcontrol)
        m_flowcontrol->Shutdown();
    if (m_netStatus)
        m_netStatus->Shutdown();

    m_netsocket.reset();
}

bool NetClient::NetDebugInit()
{
    m_netStatus = std::make_unique<NetStatus>(this);

    if (!m_netsocket)
        return false;

    m_netsocket->OnReceive().Connection(&NetStatus::SlotOnReceive, m_netStatus.get());
    m_netsocket->OnSend().Connection(&NetStatus::SlotOnSend, m_netStatus.get());
    m_netStatus->OnReportPing().Connection(&NetClient::SlotReportPing, this);
    return true;
}

void NetClient::SlotReceivePacket(std::unique_ptr<NetPacket> &&packet)
{
    if (m_flowcontrol)
        m_flowcontrol->Enqueue(std::move(packet), NetFlowControl::IOType::IN);
}

void NetClient::ClientSendEcho()
{
    if (m_flowcontrol)
        m_flowcontrol->SendEchoToServer("echo test message");
}

void NetClient::ClientTestSendFileRequest(const std::string &req)
{
    if (m_flowcontrol)
        m_flowcontrol->TestSendFilePacket(req);
}

void NetClient::ClientSendChat(const std::string &say)
{
    if (m_flowcontrol)
        m_flowcontrol->SendChatMessage(say);
}

bool NetClient::SetNetworkParam(const std::string &ip, const std::string &port)
{
    if (ip.empty() || port.empty())
        return false;

    if (port.length() > 5)
        return false;

    for (const auto &c : port)
    {
        switch (c)
        {
        case '0': case '1':case '2':case '3':case '4':
        case '5':case '6':case '7':case '8':case '9':
            break;
        default:
            return false;
        }
    }
    std::stringstream ss(port);
    uint16_t portId = 0;

    ss >> portId;

    SetNetOption(ip, portId);
    return true;
}

void NetClient::RegistInnerPacketListener(NetService *listener, std::function<void(std::shared_ptr<NetPacket>&&)> &&invokable)
{
    m_flowcontrol->OnReleaseInnerPacket().Connection(std::forward<std::remove_reference<decltype(invokable)>::type>(invokable), listener);
}

void NetClient::SlotReportPing(uint32_t recvCount, uint32_t sendCount)
{
    std::string report = stringFormat("server not response (receive: %d, send: %d)", recvCount, sendCount);

    NET_PUSH_LOGMSG(report, PrintUtil::ConsoleColor::COLOR_GREY);
    if (m_flowcontrol)
        m_flowcontrol->DebugReportInputOutputCounting();

    if (m_receiver)
        m_receiver->DebugShowReceive();
}