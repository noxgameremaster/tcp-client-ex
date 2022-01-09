
#ifndef NET_STATUS_H__
#define NET_STATUS_H__

#include "netservice.h"

class LoopThread;

class NetStatus : public NetService
{
    static constexpr uint32_t ping_time_rate = 300;
private:
    std::unique_ptr<LoopThread> m_statusThread;
    uint32_t m_recvCount;
    uint32_t m_sendCount;
    uint32_t m_lastIoTime;

    NetObject *m_parent; //debug only

public:
    explicit NetStatus(NetObject *parent);
    ~NetStatus() override;

private:
    bool CheckNetStatus();
    bool OnInitialize() override;
    bool OnStarted() override;
    void OnDeinitialize() override;
    void OnStopped() override;

public:
    void SlotOnReceive(uint32_t recvCount);
    void SlotOnSend(uint32_t sendCount);

private:
    DECLARE_SIGNAL(OnReportPing, uint32_t, uint32_t)

private:
    std::mutex m_lock;
};

#endif

