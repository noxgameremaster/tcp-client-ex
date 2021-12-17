
#include "clientworker.h"
#include "netclient.h"
#include "iobuffer.h"
#include "localbuffer.h"
#include "loopThread.h"
#include "packetproducer.h"

ClientWorker::ClientWorker(NetObject *parent)
    : NetService(parent)
{
    m_workThread = std::make_unique<LoopThread>();
    m_workThread->SetTaskFunction([this]() { this->FetchFromBuffer(); });
    m_workThread->SetWaitCondition([this]() { return !(this->m_recvbuffer->IsEmpty()); });
}

ClientWorker::~ClientWorker()
{ }

void ClientWorker::InterceptPacket(std::unique_ptr<NetPacket> &&pack)
{
    m_OnReleasePacket.Emit(std::move(pack));
}

void ClientWorker::FillLocalBuffer()
{
    size_t readsize = 0;

    while (true)
    {
        std::unique_ptr<uint8_t[]> alloc;

        if (!m_recvbuffer->PopBufferAlloc(std::move(alloc), readsize))
            break;

        if (!m_localbuffer->Append(alloc.get(), readsize))
            break;
    }
}

void ClientWorker::FetchFromBuffer()
{
    FillLocalBuffer();

    while (!m_localbuffer->IsEmpty())
    {
        if (m_produce->ReadBuffer())
            m_produce->MakePacket();
    }
}

void ClientWorker::BufferOnPushed()
{
    m_workThread->Notify();
}

void ClientWorker::SetReceiveBuffer(std::shared_ptr<IOBuffer> recvBuffer)
{
    m_recvbuffer = recvBuffer;
}

bool ClientWorker::InitPacketForwarding()
{
    NetObject *parent = GetParent();

    if (nullptr == parent)
        return false;
    NetClient *serv = dynamic_cast<NetClient *>(parent);

    if (nullptr == serv)
        return false;

    return m_OnReleasePacket.Connection(&NetClient::SlotReceivePacket, serv);
}

bool ClientWorker::OnInitialize()
{
    if (!m_recvbuffer)
        return false;
    if (!InitPacketForwarding())
        return false;

    m_produce = std::make_unique<PacketProducer>();
    m_localbuffer = std::make_shared<LocalBuffer>();
    m_produce->SetLocalBuffer(m_localbuffer);
    m_recvbuffer->SetTrigger(this, std::bind(&ClientWorker::BufferOnPushed, this));
    return true;
}

void ClientWorker::OnDeinitialize()
{
    if (m_recvbuffer)
        m_recvbuffer.reset();
}

bool ClientWorker::OnStarted()
{
    m_produce->SetCapture(this,
        [this](std::unique_ptr<NetPacket> &&p)
    { this->InterceptPacket(std::forward<std::remove_reference<decltype(p)>::type>(p)); });

    return m_workThread->Startup();
}

void ClientWorker::HaltWorkThread()
{
    m_workThread->Shutdown();
}

void ClientWorker::OnStopped()
{
    HaltWorkThread();
}



