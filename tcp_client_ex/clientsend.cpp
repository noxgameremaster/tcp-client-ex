#include "clientsend.h"
#include "netclient.h"
#include "netflowcontrol.h"
#include "iobuffer.h"
#include "eventThread.h"
#include "winsocket.h"

ClientSend::ClientSend(NetObject *parent)
    : NetService(parent), m_lock(new std::mutex)
{
    m_sendbuffer = std::make_shared<IOBuffer>();
    m_sendbuffer->SetLargeBufferScale(IOBuffer::receive_buffer_max_size);
    m_sendbuffer->SetTrigger(this, [this]() { this->BufferOnPushed(); });
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

void ClientSend::OnInitialOnce()
{
    m_sendThread = std::make_unique<EventThread>(this);
    m_sendThread->SetCondition([this]() { return !this->m_sendbuffer->IsEmpty(); });
    m_sendThread->SetExecution([this]() { return this->StreamSend(); });
    m_sendThread->SetLocker(m_lock);
}

bool ClientSend::OnStarted()
{
    return m_netsocket ? m_sendThread->Startup() : false;
}

void ClientSend::OnStopped()
{
    m_sendThread->Shutdown();
}

void ClientSend::SetSendSocket(std::shared_ptr<WinSocket> &sock)
{
    m_netsocket = sock;
}