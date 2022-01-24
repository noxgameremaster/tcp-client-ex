
#ifndef CLIENT_WORKER_H__
#define CLIENT_WORKER_H__

#include "netservice.h"
#include <vector>

class NetPacket;
class EventThread;
class PacketBufferFix;
class WinSocket;

class ClientWorker : public NetService
{
private:
    std::unique_ptr<EventThread> m_workThread;
    std::unique_ptr<PacketBufferFix> m_packetBuffer;

public:
    explicit ClientWorker(NetObject *parent);
    ~ClientWorker() override;

private:
    bool IsContained() const;
    bool FetchFromBuffer();

public:
    bool PushWorkBuffer(WinSocket *sock, const std::vector<uint8_t> &stream);

private:
    bool InitPacketForwarding();
    void OnInitialOnce() override;
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

public:
    void SlotWorkerWakeup();
    size_t GetWorkBufferSize() const;

private:
    DECLARE_SIGNAL(OnReleasePacket, std::unique_ptr<NetPacket>&&)

private:
    std::shared_ptr<std::mutex> m_lock;
};

#endif

