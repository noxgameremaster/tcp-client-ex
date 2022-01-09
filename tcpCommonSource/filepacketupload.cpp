
#include "filepacketupload.h"
#include "packetOrderTable.h"

#include <array>

FilePacketUpload::FilePacketUpload()
    : NetPacket()
{
    m_pathLength = 0;
}

FilePacketUpload::~FilePacketUpload()
{ }

size_t FilePacketUpload::PacketSize(Mode mode)
{
    if (Mode::Write == mode)
    {
        switch (SubCommand())
        {
        case PacketSubCmd::ToFileServer:
        case PacketSubCmd::FileServerToClient:
            return sizeof(m_pathLength) + m_pathLength;
        case PacketSubCmd::TestSendToServer:
            return 255;
        }
    }

    return 0;
}

bool FilePacketUpload::ReadSendTesting()
{
    try
    {
        ReadCtx(m_pathLength);
        
        m_uploadPath.resize(static_cast<size_t>(m_pathLength));
        for (auto &c : m_uploadPath)
            ReadCtx(c);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool FilePacketUpload::ReadFromFileServer()
{
    return true;
}

bool FilePacketUpload::OnReadPacket()
{
    switch (SubCommand())
    {
    case PacketSubCmd::TestSendToServer: return ReadSendTesting();
    case PacketSubCmd::FileServerToClient: return ReadFromFileServer();
    case PacketSubCmd::ToFileServer: return ReadSendTesting();
    default: return false;
    }
}

bool FilePacketUpload::TestSendWrite()
{
    std::array<char, 255> buff;

    buff.fill(0);
    std::copy(m_uploadPath.cbegin(), m_uploadPath.cend(), buff.begin());

    try
    {
        for (const auto &c : buff)
            WriteCtx(c);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool FilePacketUpload::ToFileServerWrite()
{
    try
    {
        WriteCtx(m_pathLength);
        for (const auto &c : m_uploadPath)
            WriteCtx(c);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool FilePacketUpload::OnWritePacket()
{
    switch (SubCommand())
    {
    case PacketSubCmd::TestSendToServer: return TestSendWrite();
    case PacketSubCmd::FileServerToClient:
    case PacketSubCmd::ToFileServer: /*return TestSendWrite();*/ return ToFileServerWrite();
    default: return false;
    }
}

uint8_t FilePacketUpload::GetPacketId() const
{
    return static_cast<uint8_t>(PacketOrderTable<FilePacketUpload>::GetId());
}

void FilePacketUpload::SetUploadPath(const std::string &upPath)
{
    m_pathLength = static_cast<decltype(m_pathLength)>(upPath.length());

    //decltype(m_pathLength) rep = 0;

    m_uploadPath = upPath.substr(0, static_cast<size_t>(m_pathLength));
}

void FilePacketUpload::ChangeSubCommand(FilePacketUpload::PacketSubCmd subcmd)
{
    auto setter = [](PacketSubCmd sub)->uint8_t
    {
        switch (sub)
        {
        case PacketSubCmd::ToFileServer: return 1;
        case PacketSubCmd::FileServerToClient: return 2;
        case PacketSubCmd::SendToServer: return 100;
        case PacketSubCmd::SendToClient: return 101;
        case PacketSubCmd::TestSendToServer: return 102;
        default: return 0;
        }
    };    
    SetSubCommand(setter(subcmd));
}

FilePacketUpload::PacketSubCmd FilePacketUpload::SubCommand() const
{
    auto getter = [](uint8_t sub)
    {
        switch (sub)
        {
        case 1: return FilePacketUpload::PacketSubCmd::ToFileServer;
        case 2: return FilePacketUpload::PacketSubCmd::FileServerToClient;
        case 100: return FilePacketUpload::PacketSubCmd::SendToServer;
        case 101: return FilePacketUpload::PacketSubCmd::SendToClient;
        case 102: return FilePacketUpload::PacketSubCmd::TestSendToServer;
        default: return FilePacketUpload::PacketSubCmd::None;
        }
    };
    return getter(GetSubCommand());
}