
#include "netpacket.h"
#include "headerdata.h"
#include "netLogObject.h"
#include "stringHelper.h"

using namespace _StringHelper;

NetPacket::NetPacket()
    : BinaryStream(32786)
{
    m_senderSocketId = static_cast<socket_type>(-1);
    m_headerData = std::make_unique<HeaderData>();
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
    SetSeekpoint(m_headerData->DataSectionOffset());
    if (!OnReadPacket())
        return PacketError(Mode::Read);

    return true;
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
    size_t sizeAll = m_headerData->FieldLength() + PacketSize(Mode::Write);

    m_headerData->SetProperty<HeaderData::FieldInfo::LENGTH>(sizeAll);
    m_headerData->SetProperty<HeaderData::FieldInfo::MAIN_CMD_TYPE>(GetPacketId());
    BufferResize(sizeAll);    //@brief. 패킷의 총 길이를 먼저 설정합니다

    if (!WriteHeaderData())
        return PacketError(Mode::Write);

    if (!OnWritePacket())
        return PacketError(Mode::Write);

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

void NetPacket::SetSubCommand(uint8_t subCmd)
{
    m_headerData->SetProperty<HeaderData::FieldInfo::SUB_CMD_TYPE>(subCmd);
}

uint8_t NetPacket::GetSubCommand() const
{
    uint8_t subCmd = static_cast<uint8_t>(-1);

    m_headerData->GetProperty<HeaderData::FieldInfo::SUB_CMD_TYPE>(subCmd);
    return subCmd;
}

bool NetPacket::PacketError(Mode workType)
{
    NetLogObject::LogObject().AppendLogMessage(stringFormat("task: %s, packet error in %s", 
        (workType == Mode::Read) ? "read" : (workType==Mode::Write ? "write" : "none"), ClassName()),
        PrintUtil::ConsoleColor::COLOR_DARKRED);

    return false;
}