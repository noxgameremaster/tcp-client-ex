
#include "clientreceive.h"
#include "netflowcontrol.h"
#include "clientworker.h"
#include "socketset.h"
#include "winsocket.h"
#include "netLogObject.h"
#include "loopThread.h"
#include "eventworker.h"
#include "stringHelper.h"
#include <assert.h>
#include <ws2tcpip.h>

using namespace _StringHelper;

ClientReceive::ClientReceive(NetObject *parent)
    : NetService(parent)
{
    static_assert(read_receive_buffer_count >= 4, "a receive count must be equal or greator than 4");

    m_receiveThread = std::make_unique<LoopThread>(this);
    m_readFds = std::make_unique<SocketSet>();
    m_readFds->SetTimeInterval(1, 0);
}

ClientReceive::~ClientReceive()
{
    m_networker.reset();
}

void ClientReceive::OnInitialOnce()
{
    m_networker = std::make_unique<ClientWorker>(GetParent());

    m_OnReceivePushStream.Connection(&ClientWorker::SlotWorkerWakeup, m_networker.get());
}

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
    NET_PUSH_LOGMSG("the receive buffer is fully", PrintUtil::ConsoleColor::COLOR_RED);

    return false;
}

void ClientReceive::OnDisconnected(WinSocket *sock)
{
    NotifyErrorToOwner();
    NET_PUSH_LOGMSG(stringFormat("the user %d has disconnect with me", sock->GetFd()), PrintUtil::ConsoleColor::COLOR_RED);
    
    m_readFds->Remove(sock);
    m_OnDisconnect.Emit(sock->GetFd());
}

bool ClientReceive::ReceiveFrom(WinSocket *sock)
{
    std::vector<uint8_t> receiveVector;

    do
    {
        receiveVector.resize(read_receive_buffer_count);
        if (!sock->Receive(receiveVector))
            OnDisconnected(sock);
        else if (!m_networker->PushWorkBuffer(sock, receiveVector))
            ErrorBufferIsFull();
        else
        {
            //QUEUE_EMIT(m_OnReceivePushStream);
            m_OnReceivePushStream.Emit();
            break;
        }
        return false;
    }
    while (false);

    return true;
}

bool ClientReceive::DoTask()
{
    return m_readFds->DoSelect([this](WinSocket *s) { return this->ReceiveFrom(s); });
}

bool ClientReceive::OnInitialize()
{
    if (!m_netsocket)
        return false;

    return m_networker->Startup();
}

void ClientReceive::OnDeinitialize()
{
    if (m_networker)
        m_networker->Shutdown();

    m_readFds->Clear();
}

bool ClientReceive::OnStarted()
{
    m_receiveThread->SetTaskFunction([this]() { return this->DoTask(); });

    return m_receiveThread->Startup();
}

void ClientReceive::OnStopped()
{
    m_receiveThread->Shutdown();
}

void ClientReceive::SetReceiveSocket(std::shared_ptr<WinSocket> &sock)
{
    m_netsocket = sock;
    m_readFds->Append(m_netsocket.get());
}

void ClientReceive::DebugShowReceive()
{
    std::string debugmsg = stringFormat("receive buffer: %d ", m_networker->GetWorkBufferSize());

    NET_PUSH_LOGMSG(debugmsg);
}
