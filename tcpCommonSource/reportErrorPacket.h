
#ifndef REPORT_ERROR_PACKET_H__
#define REPORT_ERROR_PACKET_H__

#include "netpacket.h"

class ReportErrorPacket : public NetPacket
{
private:
    int m_errCodeLow;
    int m_errCodeHigh;
    std::array<char, 64> m_errTitle;
    std::array<char, 255> m_errContent;

public:
    explicit ReportErrorPacket();
    ~ReportErrorPacket() override;

private:
    size_t PacketSize(Mode mode) override;

public:
    static std::string TaskName()
    {
        return "ReportErrorPacket";
    }

private:
    bool OnReadPacket() override;
    bool OnWritePacket() override;
    uint8_t GetPacketId() const override;

    std::string ClassName() const override
    {
        return TaskName();
    }

public:
    std::list<std::string> GetErrorInfo();
};

#endif

