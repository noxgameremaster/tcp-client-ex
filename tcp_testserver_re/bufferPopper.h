
#ifndef BUFFER_POPPER_H__
#define BUFFER_POPPER_H__

#include "netservice.h"

class PacketBuffer;
class LoopThread;
class NetPacket;

class BufferPopper : public NetService
{
private:
    std::shared_ptr<PacketBuffer> m_packetBuffer;
    std::unique_ptr<LoopThread> m_popThread;

public:
    explicit BufferPopper();
    ~BufferPopper() override;

private:
    bool PopData();
    bool OnInitialize() override;
    bool OnStarted() override;
    void OnDeinitialize() override;
    void OnStopped() override;

    static std::unique_ptr<NetPacket> DistinguishPacket(uint8_t packetId);

public:
    void SlotRegistBuffer(std::shared_ptr<PacketBuffer> buffer);

    DECLARE_SIGNAL(OnReleasePacket, std::unique_ptr<NetPacket>)
};

#endif

