
#include "netpacket.h"
#include "headerdata.h"

NetPacket::NetPacket()
    : BinaryStream(1024)
{ }

NetPacket::~NetPacket()
{ }

bool NetPacket::OnReadPacket()
{
    return false;
}

bool NetPacket::OnWritePacket()
{
    return false;
}

size_t NetPacket::PacketSize()
{
    return sizeof(m_headerData);
}

void NetPacket::SetHeaderData(std::unique_ptr<HeaderData> &&headerData)
{
    m_headerData = std::forward<std::remove_reference<decltype(headerData)>::type>(headerData);
}

bool NetPacket::Read()
{
    if (m_headerData)
        SetSeekpoint(m_headerData->DataSectionOffset());

    return OnReadPacket();
}

bool NetPacket::WriteHeaderData(const size_t length)
{
    int stx = 0;

    m_headerData->GetProperty<HeaderData::FieldInfo::STX>(stx);

    try
    {
        WriteCtx(stx);
        WriteCtx(length);
        WriteCtx(GetPacketId());
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool NetPacket::Write()
{
    if (!m_headerData)
        m_headerData = std::make_unique<HeaderData>();

    size_t sizeAll = m_headerData->FieldLength() + PacketSize();

    BufferResize(sizeAll);    //@brief. 패킷의 총 길이를 먼저 설정합니다

    if (!WriteHeaderData(sizeAll))
        return false;

    if (!OnWritePacket())
        return false;

    int etx = m_headerData->GetProperty<HeaderData::FieldInfo::ETX>(etx);
    
    return SetStreamChunk(etx, sizeAll - sizeof(etx));
}

