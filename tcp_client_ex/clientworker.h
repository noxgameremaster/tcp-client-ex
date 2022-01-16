
#ifndef CLIENT_WORKER_H__
#define CLIENT_WORKER_H__

#include "netservice.h"
#include <vector>

class NetPacket;
class LoopThread;
class PacketBufferFix;

class ClientWorker : public NetService
{
private:
    std::unique_ptr<LoopThread> m_workThread;
    std::shared_ptr<PacketBufferFix> m_packetBuffer;

public:
    explicit ClientWorker(NetObject *parent);
    ~ClientWorker() override;

private:
    bool IsContained() const;
    bool FetchFromBuffer();

public:
    void BufferOnPushed();
    void SetReceiveBuffer(std::shared_ptr<PacketBufferFix> packetBuffer);

private:
    bool InitPacketForwarding();
    bool OnInitialize() override;
    void OnDeinitialize() override;
    bool OnStarted() override;
    void HaltWorkThread();
    void OnStopped() override;

    std::string ObjectName() const override
    {
        return "ClientWorker";
    }

    static std::unique_ptr<NetPacket> DistinguishPacket(uint8_t packetId);

private:
    DECLARE_SIGNAL(OnReleasePacket, std::unique_ptr<NetPacket>&&)
};

#endif

