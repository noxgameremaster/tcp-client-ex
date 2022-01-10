
#include "clientreceive.h"
#include "netflowcontrol.h"
#include "clientworker.h"
#include "packetBufferFix.h"
#include "socketset.h"
#include "winsocket.h"
#include "netLogObject.h"
#include "loopThread.h"
#include "eventworker.h"
#include "stringHelper.h"
#include <assert.h>
#include <ws2tcpip.h>

using namespace _StringHelper;

ClientReceive::ClientReceive(std::shared_ptr<WinSocket> &sock, NetObject *parent)
    : NetService(parent)
{
    static_assert(read_receive_buffer_count >= 4, "a receive count must be equal or greator than 4");

    m_netsocket = sock;
    m_readFds = std::make_unique<SocketSet>();
    m_readFds->SetTimeInterval(1, 0);

    m_packetBuffer = std::make_unique<PacketBufferFix>();
    m_receiveThread = std::make_unique<LoopThread>();

    m_receiveThread->SetTaskFunction([this]() { return this->DoTask(); });

    m_stopped = false;  //fixme
}

ClientReceive::~ClientReceive()
{ }

bool ClientReceive::NotifyErrorToOwner()
{
    std::string errorMsg = GetErrorMessage();
    NetObject *parent = GetParent();

    if (parent == nullptr)
        return false;

    NetService *service = dynamic_cast<NetService *>(parent);

    if (service == nullptr)
        return false;

    NotifyOccurError(service, "serverError", errorMsg);
    return true;
}

bool ClientReceive::ErrorBufferIsFull()
{
    NetLogObject::LogObject().AppendLogMessage("the receive buffer is fully", PrintUtil::ConsoleColor::COLOR_RED);

    return false;
}

void ClientReceive::OnDisconnected(WinSocket *sock)
{
    NotifyErrorToOwner();
    NetLogObject::LogObject().AppendLogMessage(stringFormat("the user %d has disconnect with me", sock->GetFd()), 
        PrintUtil::ConsoleColor::COLOR_RED);
    
    m_readFds->Remove(sock);
    m_OnDisconnect.Emit(sock->GetFd());
}

void ClientReceive::ReceiveFrom(WinSocket *sock)
{
    std::vector<uint8_t> receiveVector(read_receive_buffer_count, 0);

    do
    {
        if (!sock->Receive(receiveVector))
            OnDisconnected(sock);
        else if (!m_packetBuffer->PushBack(sock, receiveVector))
            ErrorBufferIsFull();
        else
        {
            EventWorker::Instance().AppendTask(&m_OnReceivePushStream);
            break;
        }
        m_stopped = true;
    }
    while (false);
}

bool ClientReceive::DoTask()
{
    m_readFds->DoSelect([this](WinSocket *s) { this->ReceiveFrom(s); });

    return !m_stopped;
}

bool ClientReceive::OnInitialize()
{
    if (!m_netsocket)
        return false;

    m_readFds->Append(m_netsocket.get());

    m_networker = std::make_unique<ClientWorker>(GetParent());
    m_networker->SetReceiveBuffer(m_packetBuffer);
    m_networker->Startup();

    m_OnReceivePushStream.Connection(&ClientWorker::BufferOnPushed, m_networker.get());

    return true;
}

void ClientReceive::OnDeinitialize()
{

    if (m_networker)
    {
        m_networker->Shutdown();
        m_networker.reset();
    }
}

bool ClientReceive::OnStarted()
{
    return m_receiveThread->Startup();
}

void ClientReceive::OnStopped()
{
    m_receiveThread->Shutdown();
}



