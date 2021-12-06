
#ifndef CLIENT_WORKER_H__
#define CLIENT_WORKER_H__

#include "netservice.h"
#include <mutex>
#include <thread>
#include <vector>
#include <future>

class IOBuffer;
class LocalBuffer;
class PacketProducer;
class NetPacket;
class NetFlowControl;

class ClientWorker : public NetService
{
    static constexpr size_t local_buffer_size = 1024;
private:
    bool m_terminated;
    std::shared_ptr<IOBuffer> m_recvbuffer;
    std::condition_variable m_condvar;
    std::thread m_workerThread;

    std::shared_ptr<LocalBuffer> m_localbuffer;

    std::future<bool> m_workResult;

    std::unique_ptr<PacketProducer> m_analyzer;
    std::weak_ptr<NetFlowControl> m_flowcontrol;

public:
    explicit ClientWorker(NetObject *parent = nullptr);
    ~ClientWorker() override;

private:
    void InterceptPacket(std::unique_ptr<NetPacket> &&pack);
    bool FetchFromBuffer();
    bool DoTask();
    void BufferOnPushed();

public:
    void SetReceiveBuffer(std::shared_ptr<IOBuffer> recvBuffer);

private:
    bool OnInitialize() override;
    void OnDeinitialize() override;
    bool OnStarted() override;
    void HaltWorkThread();
    void OnStopped() override;

private:
    std::mutex m_waitlock;
};

#endif

