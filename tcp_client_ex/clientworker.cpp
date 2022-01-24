
#include "clientworker.h"
#include "netclient.h"
#include "iobuffer.h"
#include "packetBufferFix.h"
#include "chatPacket.h"
#include "echoPacket.h"
#include "filepacket.h"
#include "fileChunkPacket.h"
#include "largeFileChunkPacket.h"
#include "largeFileCompletePacket.h"
#include "largeFileRequestPacket.h"
#include "reportErrorPacket.h"
#include "filepacketupload.h"
#include "packetOrderTable.h"
#include "eventThread.h"

ClientWorker::ClientWorker(NetObject *parent)
    : NetService(parent), m_lock(new std::mutex)
{
    m_workThread = std::make_unique<EventThread>(this);
    m_workThread->SetExecution([this]() { return this->FetchFromBuffer(); });
    m_workThread->SetCondition([this]() { return this->IsContained(); });
    m_workThread->SetLocker(m_lock);

    m_packetBuffer = std::make_unique<PacketBufferFix>();
    m_packetBuffer->SetInstanceFunction([](uint8_t packetId)
    { return ClientWorker::DistinguishPacket(packetId); });
}

ClientWorker::~ClientWorker()
{ }

bool ClientWorker::IsContained() const
{
    return !m_packetBuffer->IsEmpty();
}

bool ClientWorker::FetchFromBuffer()
{
    std::unique_ptr<NetPacket> pack = nullptr;

    {
        std::unique_lock<std::mutex> lock(*m_lock);
        m_packetBuffer->PopPacket(pack);
    }

    if (!pack)
        return true;

    m_OnReleasePacket.Emit(std::move(pack));
    return true;
}

bool ClientWorker::PushWorkBuffer(WinSocket *sock, const std::vector<uint8_t> &stream)
{
    {
        std::unique_lock<std::mutex> lock(*m_lock);

        if (!m_packetBuffer->PushBack(sock, stream))
            return false;
    }
    //m_workThread->Notify();
    return true;
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

void ClientWorker::OnInitialOnce()
{
    InitPacketForwarding();
}

bool ClientWorker::OnInitialize()
{
    return m_packetBuffer ? true : false;
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
    case PacketOrderTable<LargeFileChunkPacket>::GetId(): return std::make_unique<LargeFileChunkPacket>();
    case PacketOrderTable<LargeFileCompletePacket>::GetId(): return std::make_unique<LargeFileCompletePacket>();
    case PacketOrderTable<LargeFileRequestPacket>::GetId(): return std::make_unique<LargeFileRequestPacket>();
    case PacketOrderTable<ReportErrorPacket>::GetId(): return std::make_unique<ReportErrorPacket>();
    default: return nullptr;
    }
}

void ClientWorker::SlotWorkerWakeup()
{
    m_workThread->Notify();
}

size_t ClientWorker::GetWorkBufferSize() const
{
    return m_packetBuffer->DebugPacketBufferSize();
}

