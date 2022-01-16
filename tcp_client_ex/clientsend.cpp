#include "clientsend.h"
#include "netclient.h"
#include "netflowcontrol.h"
#include "iobuffer.h"
#include "loopThread.h"
#include "winsocket.h"

ClientSend::ClientSend(std::shared_ptr<WinSocket> &sock, NetObject *parent)
    : NetService(parent)
{
    m_netsocket = sock;
    m_sendThread = std::make_unique<LoopThread>(this);
    m_sendThread->SetTaskFunction([this]() { return this->StreamSend(); });
    m_sendThread->SetWaitCondition([this]() { return !this->m_sendbuffer->IsEmpty(); });
}

ClientSend::~ClientSend()
{ }

void ClientSend::BufferOnPushed()
{
    m_sendThread->Notify();
}

bool ClientSend::StreamSend()
{
    std::unique_ptr<uint8_t[]> stream;
    size_t buffersize = 0;

    if (!m_sendbuffer->PopBufferAlloc(std::move(stream), buffersize))
        return true;

    m_netsocket->Send(stream.get(), buffersize);
    return true;
}

bool ClientSend::OnInitialize()
{
    m_sendbuffer = std::make_shared<IOBuffer>();
    m_sendbuffer->SetLargeBufferScale(IOBuffer::receive_buffer_max_size);
    m_sendbuffer->SetTrigger(this, [this]() { this->BufferOnPushed(); });

    return true;
}

void ClientSend::OnDeinitialize()
{ }

bool ClientSend::OnStarted()
{
    return m_sendThread->Startup();
}

void ClientSend::OnStopped()
{
    m_sendThread->Shutdown();
}

