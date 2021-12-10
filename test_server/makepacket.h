
#ifndef MAKE_PACKET_H__
#define MAKE_PACKET_H__

#include "binarystream.h"
#include "ccobject.h"
#include <string>
#include <functional>
#include <mutex>

class MakePacket : public BinaryStream
{
    static constexpr int packet_stx = 0xdeadface;
    static constexpr char packet_chat_type = 1;
    static constexpr int packet_etx = 0xfadeface;
private:
    std::vector<char> m_packet;

public:
    MakePacket();
    ~MakePacket() override;
    
private:
    size_t HeaderLength() const;

public:
    bool MakeChat(const std::string &msg, const uint8_t &messageColor);
    bool MakeEcho(const std::string &echoMessage);
    bool MakeFileMeta(const std::string &filename, const std::string &path);

private:
    bool SendImpl(int sendsock, const uint8_t *stream, const size_t &length);

public: 
    bool NetSendAll(int clientsocket, const std::string &msg, const uint8_t colr=6);
    bool NetEchoSend(int clientsocket);

    template <class Function, class... Args>
    bool NetSendPacket(int sendsock, Function &&f, Args&&... args)
    {
        //std::lock_guard<std::mutex> guard(m_lock);
        if (!(this->*f)(std::forward<Args>(args)...))
            return false;

        return SendImpl(sendsock, GetRaw(), BufferSize());
    }

private:
    /*virtual void OnReceiveUnknownPacket(const char *unknownStream, const size_t &length) {}
    virtual void OnReceiveChatPacket(const std::string &msg) {}
    virtual void OnReceiveEchoPacket(const std::string &echo) {}
    virtual void UnknownPacketType(uint8_t packetId) {}*/

    bool ReadChatPacket(int senderSocket);
    bool ReadEchoPacket(int senderSocket);
    bool ReadFileMetaPacket(int senderSocket);
    bool PacketTypeCase(int senderSocket, const uint8_t type);

public:
    bool ReadPacket(int senderSocket, const char *buffer, const size_t &length);
    static std::string filterPrint(const char *str, const size_t &length);

private:
    //std::mutex m_lock;

    DECLARE_SIGNAL(OnReceiveChat, int, std::string)
    DECLARE_SIGNAL(OnReceiveEcho, int, std::string)
    DECLARE_SIGNAL(OnUnknownPacketType, int, uint8_t)
    DECLARE_SIGNAL(OnReceiveUnknown, int, std::unique_ptr<char[]>, size_t)
    DECLARE_SIGNAL(OnSended, const uint8_t *, size_t)
    DECLARE_SIGNAL(OnReceiveFileMeta, int)

};

#endif

