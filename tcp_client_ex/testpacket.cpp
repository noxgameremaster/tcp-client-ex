
#include "testpacket.h"
#include "packetordertable.h"

TestPacket::TestPacket()
{
    m_magic = 0xdeadface;
}

TestPacket::~TestPacket()
{
}

size_t TestPacket::PacketSize(Mode)
{
    return sizeof(m_magic);
}

bool TestPacket::OnReadPacket()
{
    if (!GetStreamChunk(m_magic, 0))
        return false;

    return true;
}

uint8_t TestPacket::GetPacketId() const
{
    return static_cast<uint8_t>(PacketOrderTable<TestPacket>::GetId());
}

