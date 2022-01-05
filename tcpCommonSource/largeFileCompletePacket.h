
#ifndef LARGE_FILE_COMPLETE_PACKET_H__
#define LARGE_FILE_COMPLETE_PACKET_H__

#include "netpacket.h"

class LargeFileCompletePacket : public NetPacket
{
private:
    uint32_t m_fileSizeLow;
    uint32_t m_fileSizeHigh;
    uint32_t m_fileCrc;

public:
    explicit LargeFileCompletePacket();
    ~LargeFileCompletePacket() override;

private:
    size_t PacketSize(Mode mode) override;
    bool WriteReportToServer();
    bool OnWritePacket() override;

public:
    static std::string TaskName()
    {
        return "LargeFileCompletePacket";
    }

private:
    std::string ClassName() const override
    {
        return TaskName();
    }

    bool OnReadPacket() override;
    uint8_t GetPacketId() const override;

public:
    void SetSubCmd();
    void SetLargeFileSize(const uint64_t &totalSize, uint32_t crc);
};

#endif

