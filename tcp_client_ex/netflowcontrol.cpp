
#include "netflowcontrol.h"
#include "taskmanager.h"
#include "iobuffer.h"
#include "netpacket.h"

NetFlowControl::NetFlowControl()
    : NetService()
{
    m_terminated = false;
    m_taskmanager = std::make_shared<TaskManager>(this);
}

NetFlowControl::~NetFlowControl()
{ }

void NetFlowControl::CheckIOList()
{
    while (m_inpacketList.size())
    {
        std::unique_ptr<NetPacket> packet = std::move(m_inpacketList.front());

        ReceivePacket(std::move(packet));
        m_inpacketList.pop_front();
    }
    while (m_outpacketList.size())
    {
        std::unique_ptr<NetPacket> packet = std::move(m_outpacketList.front());

        ReleasePacket(std::move(packet));
        m_outpacketList.pop_front();
    }
}

void NetFlowControl::IOThreadIntervalTask()
{
    do
    {
        if (m_terminated)
            break;

        {
            std::lock_guard<std::mutex> lock(m_lock);

            CheckIOList();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    while (true);
}

void NetFlowControl::StopIOThread()
{    
    m_terminated = true;

    if (m_ioThread.joinable())
        m_ioThread.join();
}

bool NetFlowControl::OnInitialize()
{
    m_ioThread = std::thread([this]() { this->IOThreadIntervalTask(); });

    return true;
}

void NetFlowControl::OnDeinitialize()
{
}

bool NetFlowControl::OnStarted()
{
    m_taskmanager->Startup();
    return true;
}

void NetFlowControl::OnStopped()
{
    StopIOThread();
    m_taskmanager->Shutdown();
}

void NetFlowControl::ReceivePacket(std::unique_ptr<NetPacket> &&packet)
{
    m_taskmanager->InputTask(std::move(packet));
}

bool NetFlowControl::ReleasePacket(std::unique_ptr<NetPacket> &&packet)
{
    if (m_sendbuffer.expired())
        return false;

    auto sendbuffer = m_sendbuffer.lock();

    std::unique_ptr<NetPacket> sendData = std::forward<std::remove_reference<decltype(packet)>::type>(packet);
    uint8_t *stream = nullptr;
    size_t length = 0;

    if (!sendData->Write(stream, length))
        return false;

    return sendbuffer->PushBuffer(stream, length);
}

void NetFlowControl::Enqueue(std::unique_ptr<NetPacket>&& packet, IOType ioType)
{
    std::list<std::remove_reference<decltype(packet)>::type> *ioList = nullptr;

    do
    {
        if (ioType == IOType::IN)
            ioList = &m_inpacketList;
        else if (ioType == IOType::OUT)
            ioList = &m_outpacketList;
        else
            break;

        {
            std::lock_guard<std::mutex> lock(m_lock);

            ioList->push_back(std::move(packet));
        }
    }
    while (false);
}

