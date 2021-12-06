
#ifndef PACKET_ORDER_TABLE_H__
#define PACKET_ORDER_TABLE_H__

template <class Ty>
class PacketOrderTable
{
private:
    static int s_id;

public:
    static int GetId()
    {
        return s_id;
    }
};

#endif

