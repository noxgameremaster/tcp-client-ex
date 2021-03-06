
#ifndef ECHO_PACKET_H__
#define ECHO_PACKET_H__

#include "netpacket.h"

class EchoPacket : public NetPacket
{
private:
    uint8_t m_messageLength;
    std::string m_message;

public:
    EchoPacket();
    ~EchoPacket();

private:
    bool ReadEchoMessage();
    bool OnReadPacket() override;
    bool OnWritePacket() override;
    uint8_t GetPacketId() const override;
    //void DoAction() override;

public:
    static std::string TaskName()
    {
        return "EchoPacket";
    }
    std::string GetEchoMessage() const
    {
        return m_message;
    }

    void SetEchoMessage(const std::string &echoMessage);

private:
    std::string ClassName() const override
    {
        return TaskName();
    }
    size_t PacketSize(Mode) override;
};

#endif

