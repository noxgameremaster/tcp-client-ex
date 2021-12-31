
#ifndef PACKET_ORDER_TABLE_H__
#define PACKET_ORDER_TABLE_H__

class ChatPacket;
class EchoPacket;
class TestPacket;
class FilePacket;
class FilePacketUpload;
class FileChunkPacket;
class DownloadCompletePacket;

#ifndef SET_PACKET_ORDER
#define SET_PACKET_ORDER(_Ty, val) \
    template <> \
    struct PacketGetId<_Ty> \
    {   \
        static constexpr int p_id=val; \
    };

template <class Ty>
class PacketOrderTable
{
private:
    template <class Ty>
    struct PacketGetId
    { };

    SET_PACKET_ORDER(ChatPacket, 1);
    SET_PACKET_ORDER(FilePacketUpload, 2);
    SET_PACKET_ORDER(FilePacket, 3);
    SET_PACKET_ORDER(FileChunkPacket, 4);
    SET_PACKET_ORDER(DownloadCompletePacket, 5);
    SET_PACKET_ORDER(TestPacket, 6);
    SET_PACKET_ORDER(EchoPacket, 7);

public:
    static constexpr int GetId()
    {
        return PacketGetId<Ty>::p_id;
    }
};

#undef SET_PACKET_ORDER
#endif

#endif

