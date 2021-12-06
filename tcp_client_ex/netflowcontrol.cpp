
#include "netflowcontrol.h"
#include "taskmanager.h"
#include "netpacket.h"

NetFlowControl::NetFlowControl()
    : NetService()
{
    m_taskmanager = std::make_shared<TaskManager>(this);
}

NetFlowControl::~NetFlowControl()
{ }

bool NetFlowControl::OnInitialize()
{
    return false;
}

void NetFlowControl::OnDeinitialize()
{
}

bool NetFlowControl::OnStarted()
{
    return false;
}

void NetFlowControl::OnStopped()
{
}

void NetFlowControl::ReceivePacket(std::unique_ptr<NetPacket> &&packet)
{
    m_taskmanager->InputTask(std::move(packet));
}
