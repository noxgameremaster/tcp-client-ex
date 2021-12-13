#include "clientsend.h"
#include "netclient.h"
#include "netflowcontrol.h"
#include "iobuffer.h"
#include "winsocket.h"

ClientSend::ClientSend(std::shared_ptr<WinSocket> &sock, NetObject *parent)
    : NetService(parent)
{
    m_terminated = false;
    m_netsocket = sock;
}

ClientSend::~ClientSend()
{ }

void ClientSend::BufferOnPushed()
{
    m_condvar.notify_one();
}

void ClientSend::StreamSend()
{
    const uint8_t *stream = nullptr;
    size_t buffersize = 0;

    if (!m_sendbuffer->PopBuffer(stream, buffersize))
        return;

    m_netsocket->Send(stream, buffersize);
}

void ClientSend::DoTask()
{
    do
    {
        {
            std::unique_lock<std::mutex> waitlock(m_waitLock);
            m_condvar.wait(waitlock, [this]() { return (m_terminated || (!m_sendbuffer->IsEmpty())); });
        }
        if (m_terminated)
            break;

        StreamSend();
    }
    while (true);
}

void ClientSend::HaltSendThread()
{
    m_terminated = true;
    m_condvar.notify_one();

    if (m_sendThread.joinable())
        m_sendThread.join();
}

bool ClientSend::OnInitialize()
{
    m_sendbuffer = std::make_shared<IOBuffer>();
    m_sendbuffer->SetLargeBufferScale(IOBuffer::receive_buffer_max_size);
    m_sendbuffer->SetTrigger(this, [this]() { this->BufferOnPushed(); });

    NetObject *parent = GetParent();

    if (parent == nullptr)
        return false;

    NetClient *client = dynamic_cast<NetClient *>(parent);
    if (client == nullptr)
        return false;

    auto flowcontrol = client->FlowControl();

    if (!flowcontrol.expired())
        flowcontrol.lock()->SetSendBuffer(m_sendbuffer);

    return true;
}

void ClientSend::OnDeinitialize()
{
}

bool ClientSend::OnStarted()
{
    m_sendThread = std::thread([this]() { this->DoTask(); });

    return true;
}

void ClientSend::OnStopped()
{
    HaltSendThread();
}

