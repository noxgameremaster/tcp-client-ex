
#ifndef NET_FLOW_CONTROL_H__
#define NET_FLOW_CONTROL_H__

#include "netservice.h"

class TaskManager;
class IOBuffer;
class NetPacket;

class NetFlowControl : public NetService
{
private:
    std::shared_ptr<TaskManager> m_taskmanager;
    std::shared_ptr<IOBuffer> m_sendbuffer;

public:
    NetFlowControl();
    ~NetFlowControl();

private:
    virtual bool OnInitialize();
    virtual void OnDeinitialize();
    virtual bool OnStarted();
    virtual void OnStopped();

public:
    void SetSendBuffer(std::shared_ptr<IOBuffer> sendbuffer)
    {
        m_sendbuffer = sendbuffer;
    }

    void ReceivePacket(std::unique_ptr<NetPacket> &&packet);
    
};

#endif

