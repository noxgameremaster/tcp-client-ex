
#include "packetProto.h"

PacketProto::PacketProto()
    : BinaryStream(1)
{ }

PacketProto::~PacketProto()
{ }

bool PacketProto::ReadNetStream(const std::vector<uint8_t> &stream)
{
    PutStream(stream);

    size_t pos = 0, max = stream.size();
    int stx = 0;

    while (pos < max)
    {
        if (!GetStreamChunk(stx, pos))
            break;

        if (stx != signiture_stx)
        {
            ++pos;
            continue;
        }


    }
    return true;;
}


