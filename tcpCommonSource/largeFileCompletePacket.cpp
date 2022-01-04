
#include "largeFileCompletePacket.h"
#include "packetOrderTable.h"

LargeFileCompletePacket::LargeFileCompletePacket()
    : NetPacket()
{
    m_fileSizeLow = 0;
    m_fileSizeHigh = 0;
    m_fileCrc = 0;
}

LargeFileCompletePacket::~LargeFileCompletePacket()
{ }

size_t LargeFileCompletePacket::PacketSize(Mode mode)
{
    if (mode != Mode::Write)
        return 0;

    return sizeof(m_fileCrc) + sizeof(m_fileSizeHigh) + sizeof(m_fileSizeLow);
}

bool LargeFileCompletePacket::WriteReportToServer()
{
    try
    {
        WriteCtx(m_fileSizeLow);
        WriteCtx(m_fileSizeHigh);
        WriteCtx(m_fileCrc);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool LargeFileCompletePacket::OnWritePacket()
{
    return WriteReportToServer();
}

bool LargeFileCompletePacket::OnReadPacket()
{
    return true;
}

uint8_t LargeFileCompletePacket::GetPacketId() const
{
    return static_cast<uint8_t>(PacketOrderTable<LargeFileCompletePacket>::GetId());
}

