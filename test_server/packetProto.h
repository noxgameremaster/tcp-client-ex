
#ifndef PACKET_PROTO_H__
#define PACKET_PROTO_H__

#include "binarystream.h"

class PacketProto : public BinaryStream
{
    static constexpr int signiture_stx = 0xdeadface;
    static constexpr int signiture_etx = 0xfadeface;
private:
    std::vector<uint8_t> m_stream;

public:
    PacketProto();
    ~PacketProto();

    bool ReadNetStream(const std::vector<uint8_t> &stream);
};

#endif

