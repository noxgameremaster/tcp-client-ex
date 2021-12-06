
#ifndef CHAT_PACKET_H__
#define CHAT_PACKET_H__

#include "netpacket.h"

class ChatPacket : public NetPacket
{
private:
    uint8_t m_messageLength;
    uint8_t m_messageColor;
    std::string m_message;

public:
    explicit ChatPacket();
    ~ChatPacket() override;

private:
    size_t PacketSize() override;
    bool ReadMessage();
    bool OnReadPacket() override;

    void DoAction() override;
    uint8_t GetPacketId() override;

public:
    std::string GetChatMessage() const
    {
        return m_message;
    }
    uint8_t GetColorId() const
    {
        return m_messageColor;
    }

    static std::string TaskName()
    {
        return "ChatPacket";
    }

private:
    std::string ClassName() const override
    {
        return TaskName();
    }
};

#endif

