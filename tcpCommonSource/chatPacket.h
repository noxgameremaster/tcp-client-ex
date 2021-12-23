
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
    size_t PacketSize(Mode) override;
    bool ReadMessage();
    bool OnReadPacket() override;
    bool OnWritePacket() override;

    void DoAction() override;
    uint8_t GetPacketId() const override;

public:
    std::string GetChatMessage() const
    {
        return m_message;
    }
    void SetChatMessage(const std::string &msg);
    uint8_t GetColorId() const
    {
        return m_messageColor;
    }
    void SetColorId(uint8_t colorId)
    {
        m_messageColor = colorId;
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

