
#include "clientworker.h"
#include "netclient.h"
#include "iobuffer.h"
#include "packetBuffer.h"
#include "chatPacket.h"
#include "echoPacket.h"
#include "filepacket.h"
#include "fileChunkPacket.h"
#include "filepacketupload.h"
#include "packetOrderTable.h"
#include "loopThread.h"

ClientWorker::ClientWorker(NetObject *parent)
    : NetService(parent)
{
    m_workThread = std::make_unique<LoopThread>();
    m_workThread->SetTaskFunction([this]() { this->FetchFromBuffer(); });
    m_workThread->SetWaitCondition([this]() { return this->IsContained(); });

    m_packetBuffer = std::make_unique<PacketBuffer>();
}

ClientWorker::~ClientWorker()
{ }

bool ClientWorker::IsContained() const
{
    if (!m_packetBuffer)
        return false;

    return !m_packetBuffer->IsEmpty();
}

void ClientWorker::FetchFromBuffer()
{
    std::unique_ptr<NetPacket> pack;

    if (m_packetBuffer->PopPacket(pack))
        m_OnReleasePacket.Emit(std::move(pack));
}

void ClientWorker::BufferOnPushed()
{
    m_workThread->Notify();
}

void ClientWorker::SetReceiveBuffer(std::shared_ptr<PacketBuffer> packetBuffer)
{
    m_packetBuffer = packetBuffer;
    m_packetBuffer->SetInstanceFunction([](uint8_t packetId) 
    { return ClientWorker::DistinguishPacket(packetId); });
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
    if (!m_packetBuffer)
        return false;
    if (!InitPacketForwarding())
        return false;

    return true;
}

void ClientWorker::OnDeinitialize()
{ }

bool ClientWorker::OnStarted()
{
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

std::unique_ptr<NetPacket> ClientWorker::DistinguishPacket(uint8_t packetId)
{
    switch (packetId)
    {
    case PacketOrderTable<ChatPacket>::GetId(): return std::make_unique<ChatPacket>();
    case PacketOrderTable<EchoPacket>::GetId(): return std::make_unique<EchoPacket>();
    case PacketOrderTable<FilePacket>::GetId(): return std::make_unique<FilePacket>();
    case PacketOrderTable<FileChunkPacket>::GetId(): return std::make_unique<FileChunkPacket>();
    case PacketOrderTable<FilePacketUpload>::GetId(): return std::make_unique<FilePacketUpload>();
    default: return nullptr;
    }
}

