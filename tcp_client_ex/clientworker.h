
#ifndef CLIENT_WORKER_H__
#define CLIENT_WORKER_H__

#include "netservice.h"
#include <vector>

class IOBuffer;
class LocalBuffer;
class PacketProducer;
class NetPacket;
class LoopThread;

class ClientWorker : public NetService
{
private:
    std::shared_ptr<IOBuffer> m_recvbuffer;
    std::shared_ptr<LocalBuffer> m_localbuffer;
    std::unique_ptr<PacketProducer> m_produce;
    std::unique_ptr<LoopThread> m_workThread;

public:
    explicit ClientWorker(NetObject *parent);
    ~ClientWorker() override;

private:
    void InterceptPacket(std::unique_ptr<NetPacket> &&pack);
    void FillLocalBuffer();
    void FetchFromBuffer();
    void BufferOnPushed();

public:
    void SetReceiveBuffer(std::shared_ptr<IOBuffer> recvBuffer);

private:
    bool InitPacketForwarding();
    bool OnInitialize() override;
    void OnDeinitialize() override;
    bool OnStarted() override;
    void HaltWorkThread();
    void OnStopped() override;

    DECLARE_SIGNAL(OnReleasePacket, std::unique_ptr<NetPacket>&&)
};

#endif

