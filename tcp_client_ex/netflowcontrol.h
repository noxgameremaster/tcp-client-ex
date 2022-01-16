
#ifndef NET_FLOW_CONTROL_H__
#define NET_FLOW_CONTROL_H__

#include "netservice.h"
#include <list>
#include <mutex>
#include <atomic>

class TaskManager;
class IOBuffer;
class NetPacket;
class LoopThread;

class NetFlowControl : public NetService
{
    using net_packet_type = std::unique_ptr<NetPacket>;
    using net_shared_packet_type = std::shared_ptr<NetPacket>;
    using net_packet_list_type = std::list<net_packet_type>;
private:
    std::unique_ptr<TaskManager> m_taskmanager; 
    std::weak_ptr<IOBuffer> m_sendbuffer;
    net_packet_list_type m_inpacketList;
    net_packet_list_type m_outpacketList;
    net_packet_list_type m_innerPacketList;
    //std::unique_ptr<LoopThread> m_ioThread;
    std::thread m_ioThread;
    std::condition_variable m_condvar;
    bool m_terminated;

    int m_debugCountIn;
    int m_debugCountOut;

    int m_debugCountEnqueueIn;
    int m_debugCountEnqueueOut;

    std::atomic<int> m_ioCount;

public:
    NetFlowControl();
    ~NetFlowControl();

private:
    void IOThreadWork();
    bool CheckHasIO() const;
    void DequeueIO(net_packet_list_type &ioList, std::function<bool(net_packet_type&&)> &&processor);
    bool CheckIOList();
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
    void ReceivePacket(net_packet_type &&packet);
    bool ReleasePacket(net_packet_type &&packet);
    void ReleaseToInnerPacket(net_packet_type &&innerPacket);

public:
    enum class IOType
    {
        IN,
        OUT,
        INNER
    };
    void Enqueue(net_packet_type &&packet, IOType ioType);
    void SendEchoToServer(const std::string &echoMsg);

    void TestSendFilePacket(const std::string &fileInfo);
    void SendChatMessage(const std::string &msg);

private:
    std::string ObjectName() const override
    {
        return "NetFlowControl";
    }

public:
    DECLARE_SIGNAL(OnReleaseInnerPacket, net_shared_packet_type)

public:
    void DebugReportInputOutputCounting();

private:
    mutable std::mutex m_lock;
};

#endif

