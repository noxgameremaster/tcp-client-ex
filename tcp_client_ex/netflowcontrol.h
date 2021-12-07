
#ifndef NET_FLOW_CONTROL_H__
#define NET_FLOW_CONTROL_H__

#include "netservice.h"
#include <thread>
#include <list>
#include <mutex>

class TaskManager;
class IOBuffer;
class NetPacket;

class NetFlowControl : public NetService
{
private:
    std::shared_ptr<TaskManager> m_taskmanager;
    std::weak_ptr<IOBuffer> m_sendbuffer;
    std::thread m_ioThread;
    bool m_terminated;
    std::list<std::unique_ptr<NetPacket>> m_inpacketList;
    std::list<std::unique_ptr<NetPacket>> m_outpacketList;

public:
    NetFlowControl();
    ~NetFlowControl();

private:
    void CheckIOList();
    void IOThreadIntervalTask();
    void StopIOThread();

    virtual bool OnInitialize();
    virtual void OnDeinitialize();
    virtual bool OnStarted();
    virtual void OnStopped();

public:
    void SetSendBuffer(std::shared_ptr<IOBuffer> sendbuffer)
    {
        m_sendbuffer = sendbuffer;
    }

private:
    void ReceivePacket(std::unique_ptr<NetPacket> &&packet);
    bool ReleasePacket(std::unique_ptr<NetPacket> &&packet);

public:
    enum class IOType
    {
        IN,
        OUT
    };
    void Enqueue(std::unique_ptr<NetPacket> &&packet, IOType ioType);

private:
    std::mutex m_lock;
};

#endif

