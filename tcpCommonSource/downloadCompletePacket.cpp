
#include "downloadCompletePacket.h"
#include "packetOrderTable.h"

DownloadCompletePacket::DownloadCompletePacket()
    : NetPacket()
{
    m_fileCrc = 0;
    m_filesizeHigh = 0;
    m_filesizeLow = 0;
}

DownloadCompletePacket::~DownloadCompletePacket()
{ }

size_t DownloadCompletePacket::PacketSize(Mode mode)
{
    if (Mode::Write == mode)
    {
        switch (SubCommand())
        {
        case SubCmd::SendToServer:
            return sizeof(m_fileCrc) + sizeof(m_filesizeHigh) + sizeof(m_filesizeLow);
        default:
            return 0;
        }
    }
    return 0;
}

bool DownloadCompletePacket::ReadDownloadComplete()
{
    try
    {
        ReadCtx(m_fileCrc);
        ReadCtx(m_filesizeLow);
        ReadCtx(m_filesizeHigh);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool DownloadCompletePacket::OnReadPacket()
{
    if (SubCommand() == SubCmd::SendToServer)
        return ReadDownloadComplete();

    return true;
}

bool DownloadCompletePacket::WriteDownloadComplete()
{
    try
    {
        WriteCtx(m_fileCrc);
        WriteCtx(m_filesizeLow);
        WriteCtx(m_filesizeHigh);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool DownloadCompletePacket::OnWritePacket()
{
    switch (SubCommand())
    {
    case SubCmd::SendToServer: return WriteDownloadComplete();
    default: return false;
    }
}

uint8_t DownloadCompletePacket::GetPacketId() const
{
    return static_cast<uint8_t>(PacketOrderTable<DownloadCompletePacket>::GetId());
}

void DownloadCompletePacket::SetFileSize(const uint64_t &size)
{
    static constexpr uint32_t dwordmask = 0xffffffff;

    m_filesizeLow = (size & dwordmask) ? (size & dwordmask) : 0;
    m_filesizeHigh = ((size >> 32) & dwordmask) ? ((size >> 32) & dwordmask) : 0;
}

void DownloadCompletePacket::ChangeSubCommand(SubCmd subcmd)
{
    auto setter = [](SubCmd sub)->uint8_t
    {
        switch (sub)
        {
        case SubCmd::SendToServer: return 1;
        case SubCmd::SendToClient: return 2;
        default: return 0;
        }
    };
    SetSubCommand(setter(subcmd));
}

DownloadCompletePacket::SubCmd DownloadCompletePacket::SubCommand() const
{
    switch (GetSubCommand())
    {
    case 1: return SubCmd::SendToServer;
    case 2: return SubCmd::SendToClient;
    default: return SubCmd::None;
    }
}