
#ifndef LARGE_FILE_REQUEST_PACKET_H__
#define LARGE_FILE_REQUEST_PACKET_H__

#include "netpacket.h"

class LargeFileRequestPacket : public NetPacket
{
public:
    enum class PacketSubCmd
    {
        None,
        StartTestToServer,
        SendToServer,
        SendToClient
    };
private:
    std::string m_requestFileUrl;
    uint8_t m_fileUrlLength;

public:
    explicit LargeFileRequestPacket();
    ~LargeFileRequestPacket() override;

private:
    size_t PacketSize(Mode mode) override;

public:
    static std::string TaskName()
    {
        return "LargeFileRequestPacket";
    }

private:
    bool ReadStartTest();
    bool OnReadPacket() override;
    bool WriteStartTest();
    bool OnWritePacket() override;
    uint8_t GetPacketId() const override;

    std::string ClassName() const override
    {
        return TaskName();
    }

public:
    void ChangeSubCommand(PacketSubCmd subcmd);
    PacketSubCmd SubCommand() const;
    void SetRequestFileUrl(const std::string &requestFileUrl);
    std::string RequestFileUrl() const
    {
        return m_requestFileUrl;
    }
};

#endif

