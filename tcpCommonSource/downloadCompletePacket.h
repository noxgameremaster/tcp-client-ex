
#ifndef DOWNLOAD_COMPLETE_PACKET_H__
#define DOWNLOAD_COMPLETE_PACKET_H__

#include "netpacket.h"

class DownloadCompletePacket : public NetPacket
{
public:
    enum class SubCmd
    {
        None,
        SendToServer,
        SendToClient
    };
private:
    uint32_t m_filesizeLow;
    uint32_t m_filesizeHigh;
    uint32_t m_fileCrc;

public:
    DownloadCompletePacket();
    ~DownloadCompletePacket() override;

private:
    size_t PacketSize(Mode mode) override;

    std::string ClassName() const override
    {
        return TaskName();
    }
    static std::string TaskName()
    {
        return "DownloadCompletePacket";
    }
    bool ReadDownloadComplete();
    bool OnReadPacket() override;
    bool WriteDownloadComplete();
    bool OnWritePacket() override;
    uint8_t GetPacketId() const override;

public:
    void SetFileSize(const uint64_t &size);
    void SetFileCrc(uint32_t crc)
    {
        m_fileCrc = crc;
    }
    void ChangeSubCommand(SubCmd subcmd);
    SubCmd SubCommand() const;
};

#endif

