
#ifndef NET_ANALYZER_H__
#define NET_ANALYZER_H__

#include "netobject.h"
#include <list>
#include <functional>

class LocalBuffer;
class NetPacket;
class HeaderData;

class PacketProducer : public NetObject
{
    using capture_function = std::function<void(std::unique_ptr<NetPacket> &&)>;

private:
    std::shared_ptr<LocalBuffer> m_localbuffer;
    std::unique_ptr<NetPacket> m_createdPacket;
    std::list<uint32_t> m_stxposList;
    std::list<uint32_t> m_tempStxposList;
    uint32_t m_ttxpos;

    std::unique_ptr<HeaderData> m_headerdata;

    std::weak_ptr<NetObject::NetObjectImpl> m_target;
    capture_function m_notifier;

public:
    PacketProducer();
    ~PacketProducer();

private:
    bool PacketPostProc();
    std::unique_ptr<NetPacket> CreatePacket(const char &packetId);

    bool MakePacketImpl(uint32_t offset);

public:
    void SetCapture(NetObject *target, capture_function &&notifier);
    void MakePacket();
    void SetLocalBuffer(std::shared_ptr<LocalBuffer> buffer);

private:
    void Scan();

public:
    bool ReadBuffer();
};

#endif

