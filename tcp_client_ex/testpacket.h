
#ifndef TEST_PACKET_H__
#define TEST_PACKET_H__

#include "netpacket.h"

class TestPacket : public NetPacket
{
private:
    uint32_t m_magic;

public:
    explicit TestPacket();
    ~TestPacket();

private:
    size_t PacketSize(Mode mode) override;
    bool OnReadPacket() override;

    std::string ClassName() const override
    {
        return "TestPacket";
    }
    uint8_t GetPacketId() override;
};

#endif

