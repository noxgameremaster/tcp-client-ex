
#ifndef LARGE_FILE_CHUNK_PACKET_H__
#define LARGE_FILE_CHUNK_PACKET_H__

#include "netpacket.h"
#include <array>

class LargeFileChunkPacket : public NetPacket
{
public:
    enum class PacketSubCmd
    {
        None,
        SendToServer,
        SendToClient,
        SentLastDataToClient
    };

private:
    std::array<char, 8192> m_fileStream;
    uint32_t m_streamLength;

public:
    explicit LargeFileChunkPacket();
    ~LargeFileChunkPacket() override;

private:
    bool OnWritePacket() override;
    size_t PacketSize(Mode mode) override;

public:
    static std::string TaskName()
    {
        return "LargeFileChunkPacket";
    }

private:
    std::string ClassName() const override
    {
        return TaskName();
    }
    bool ReadStreamFromServer();
    bool OnReadPacket() override;
    uint8_t GetPacketId() const override;

public:
    void ChangeSubCommand(PacketSubCmd subcmd);
    PacketSubCmd SubCommand() const;
    bool GetFileStream(std::vector<uint8_t> &dest);

};

#endif

