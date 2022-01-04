
#include "bufferPopper.h"
#include "loopThread.h"
#include "packetBuffer.h"
#include "chatPacket.h"
#include "echoPacket.h"
#include "filepacket.h"
#include "fileChunkPacket.h"
#include "filepacketupload.h"
#include "packetOrderTable.h"

BufferPopper::BufferPopper()
    : NetService()
{
    m_popThread = std::make_unique<LoopThread>();
    m_popThread->SetTaskFunction([this]() { this->PopData(); });
}

BufferPopper::~BufferPopper()
{ }

void BufferPopper::PopData()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    if (!m_packetBuffer)
        return;
    if (m_packetBuffer->IsEmpty())
        return;

    std::unique_ptr<NetPacket> getPacket;

    while (m_packetBuffer->PopPacket(getPacket))
    {
        ///Todo. ���⿡�� ��Ŷ�� �޾Ҵ�.
        ///���� ���� �Ѱ���� �� ���ΰ�...?
        /// �׳� ���⿡�� ó��
        m_OnReleasePacket.Emit(std::move(getPacket));
    }
}

bool BufferPopper::OnInitialize()
{
    return true;
}

bool BufferPopper::OnStarted()
{
    return m_popThread->Startup();
}

void BufferPopper::OnDeinitialize()
{ }

void BufferPopper::OnStopped()
{
    m_popThread->Shutdown();
}

std::unique_ptr<NetPacket> BufferPopper::DistinguishPacket(uint8_t packetId)
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

void BufferPopper::SlotRegistBuffer(std::shared_ptr<PacketBuffer> buffer)
{
    if (buffer)
    {
        m_packetBuffer = buffer;
        m_packetBuffer->SetInstanceFunction([](uint8_t packetId)
        { return BufferPopper::DistinguishPacket(packetId); });
    }
}
