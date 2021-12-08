
#include "clientreceive.h"
#include "netflowcontrol.h"
#include "clientworker.h"
#include "iobuffer.h"
#include "socketset.h"
#include "winsocket.h"
#include "printutil.h"
#include <ws2tcpip.h>

ClientReceive::ClientReceive(std::shared_ptr<WinSocket> &sock, NetObject *parent)
    : NetService(parent)
{
    m_netsocket = sock;
    m_terminated = false;
    m_readFds = std::make_unique<SocketSet>();
    m_readFds->SetTimeInterval(1, 0);
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

bool ClientReceive::ErrorDisconnected()
{
    NotifyErrorToOwner();

    return false;
}

bool ClientReceive::ErrorBufferIsFull()
{
    return false;
}

bool ClientReceive::Receiving()
{
    std::vector<char> receiveVector(IOBuffer::receive_buffer_max_size, 0);
    std::list<WinSocket *> notifiers = m_readFds->NotifiedClientList();

    for (WinSocket *sock : notifiers)
    {
        if (!sock->Receive(receiveVector))
            return ErrorDisconnected();             //서버가 끊어지면 -1이 들어옴

        if (!m_receivebuffer->PushBuffer(reinterpret_cast<uint8_t *>(receiveVector.data()), receiveVector.size()))
            return ErrorBufferIsFull();
    }

    return true;
}

void ClientReceive::DoTask()
{
    while (!m_terminated)
    {
        int status = select(0, m_readFds->Raw(), nullptr, nullptr, m_readFds->Interval());

        if (status == 0)
        {
            //time_out
        }
        else if (status < 0)
        {
            PrintUtil::PrintMessage("ClientReceive::DoTask - error");
            break;
        }
        else
        {
            if (!Receiving())
                break;
        }
    }
}

bool ClientReceive::OnInitialize()
{
    if (!m_netsocket)
        return false;

    m_readFds->Append(m_netsocket.get());

    m_receivebuffer = std::make_shared<IOBuffer>();
    m_receivebuffer->SetLargeBufferScale(IOBuffer::receive_buffer_max_size);

    m_networker = std::make_unique<ClientWorker>(GetParent());
    m_networker->SetReceiveBuffer(m_receivebuffer);
    m_networker->Startup();

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
    m_recvThread = std::thread([this]() { this->DoTask(); });

    return true;
}

void ClientReceive::HaltReceiveThread()
{
    m_terminated = true;

    if (m_recvThread.joinable())
        m_recvThread.join();
}

void ClientReceive::OnStopped()
{
    HaltReceiveThread();
}



