
#ifndef MAKE_PACKET_H__
#define MAKE_PACKET_H__

#include "binarystream.h"
#include <string>

class MakePacket : public BinaryStream
{
    static constexpr int packet_stx = 0xdeadface;
    static constexpr char packet_chat_type = 1;
    static constexpr int packet_etx = 0xfadeface;
private:
    std::vector<char> m_packet;

public:
    MakePacket();
    ~MakePacket();
    
private:
    size_t HeaderLength() const;
    bool Make(const std::string &msg);

public:
    bool NetSendAll(int clientsocket, const std::string &msg);
};

#endif

