
#include "testpacket.h"

TestPacket::TestPacket()
{
    m_magic = 0xdeadface;
}

TestPacket::~TestPacket()
{
}

size_t TestPacket::PacketSize()
{
    return sizeof(m_magic);
}

bool TestPacket::OnReadPacket()
{
    if (!GetStreamChunk(m_magic, 0))
        return false;

    return true;
}

