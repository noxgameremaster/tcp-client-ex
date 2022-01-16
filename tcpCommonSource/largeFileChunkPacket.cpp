
#include "largeFileChunkPacket.h"
#include "packetOrderTable.h"
#include "netLogObject.h"
#include "stringHelper.h"

using namespace _StringHelper;

LargeFileChunkPacket::LargeFileChunkPacket()
    : NetPacket(packet_unit_max_size)
{
    m_streamLength = 0;
    //m_fileStream.fill(0);
    m_fileStreamV.reserve(packet_unit_max_size);
}

LargeFileChunkPacket::~LargeFileChunkPacket()
{ }

bool LargeFileChunkPacket::OnWritePacket()
{
    switch (SubCommand())
    {
    case PacketSubCmd::SendToServer: return true;
    default: return false;
    }
}

size_t LargeFileChunkPacket::PacketSize(Mode mode)
{
    if (Mode::Write != mode)
        return 0;

    return 0;
}

bool LargeFileChunkPacket::ReadStreamFromServer()
{
    //decltype(m_streamLength) counter = 0;
    try
    {
        ReadCtx(m_streamLength);
        if (m_streamLength > NetPacket::packet_unit_max_size)
        {
            std::string errmsg = stringFormat("chunk buffer to small! receive: %d bytes", m_streamLength);

            NetLogObject::LogObject().AppendLogMessage(errmsg);
            return false;
        }
        m_fileStreamV.resize(m_streamLength);
        ReadByteArray(m_fileStreamV);
        /*while (counter < m_streamLength)
            ReadCtx(m_fileStream[counter++]);*/
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool LargeFileChunkPacket::OnReadPacket()
{
    switch (SubCommand())
    {
    case PacketSubCmd::SentLastDataToClient:
    case PacketSubCmd::SendToClient: return ReadStreamFromServer();
    default: return false;
    }
}

uint8_t LargeFileChunkPacket::GetPacketId() const
{
    return static_cast<uint8_t>(PacketOrderTable<LargeFileChunkPacket>::GetId());
}

void LargeFileChunkPacket::ChangeSubCommand(PacketSubCmd subcmd)
{
    auto getter = [](PacketSubCmd sub)->uint8_t
    {
        switch (sub)
        {
        case PacketSubCmd::SendToServer: return 1;
        case PacketSubCmd::SendToClient: return 2;
        case PacketSubCmd::SentLastDataToClient: return 4;
        default: return 0;
        }
    };
    SetSubCommand(getter(subcmd));
}

LargeFileChunkPacket::PacketSubCmd LargeFileChunkPacket::SubCommand() const
{
    switch (GetSubCommand())
    {
    case 1: return PacketSubCmd::SendToServer;
    case 2: return PacketSubCmd::SendToClient;
    case 4: return PacketSubCmd::SentLastDataToClient;
    default: return PacketSubCmd::None;
    }
}

bool LargeFileChunkPacket::GetFileStream(std::vector<uint8_t> &dest)
{
    if (!m_streamLength)
        return false;

    //dest.resize(m_streamLength);

    //std::copy_n(m_fileStream.cbegin(), m_streamLength, dest.begin());

    dest = m_fileStreamV;
    return true;
}
