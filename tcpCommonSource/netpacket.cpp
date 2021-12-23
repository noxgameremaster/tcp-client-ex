
#include "netpacket.h"
#include "headerdata.h"

NetPacket::NetPacket()
    : BinaryStream(65536)
{
    m_senderSocketId = static_cast<socket_type>(-1);
}

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

size_t NetPacket::PacketSize(Mode)
{
    return 0;
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

bool NetPacket::WriteHeaderData()
{
    std::vector<uint8_t> stream = m_headerData->ReleaseData();

    if (stream.empty())
        return false;

    try
    {
        for (const uint8_t &uc : stream)
            WriteCtx(uc);
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

    size_t sizeAll = m_headerData->FieldLength() + PacketSize(Mode::Write);

    m_headerData->SetProperty<HeaderData::FieldInfo::LENGTH>(sizeAll);
    m_headerData->SetProperty<HeaderData::FieldInfo::MAIN_CMD_TYPE>(GetPacketId());
    BufferResize(sizeAll);    //@brief. 패킷의 총 길이를 먼저 설정합니다

    if (!WriteHeaderData())
        return false;

    if (!OnWritePacket())
        return false;

    int etx = 0;

    m_headerData->GetProperty<HeaderData::FieldInfo::ETX>(etx);

    return SetStreamChunk(etx, sizeAll - sizeof(etx));
}

bool NetPacket::Write(uint8_t *&stream, size_t &length)
{
    bool result = Write();

    GetStreamInfo(stream, length);
    return result;
}

