
#include "netStatus.h"
#include "loopThread.h"
#include "eventworker.h"

NetStatus::NetStatus()
    : NetService()
{
    m_recvCount = 0;
    m_sendCount = 0;
    m_lastIoTime = 0;
}

NetStatus::~NetStatus()
{ }

bool NetStatus::CheckNetStatus()
{
    {
        std::lock_guard<std::mutex> guard(m_lock);
        if (m_lastIoTime)
        {
            --m_lastIoTime;
        }
        else
        {
            m_lastIoTime = ping_time_rate;

            EventWorker::Instance().AppendTask(&m_OnReportPing, m_recvCount, m_sendCount);
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    return true;
}

bool NetStatus::OnInitialize()
{
    if (m_statusThread)
        return false;

    m_statusThread = std::make_unique<LoopThread>();
    m_statusThread->SetTaskFunction([this]() { return this->CheckNetStatus(); });

    return true;
}

bool NetStatus::OnStarted()
{
    return m_statusThread->Startup();
}

void NetStatus::OnDeinitialize()
{
    m_statusThread->Shutdown();
    m_statusThread.reset();
}

void NetStatus::OnStopped()
{ }

void NetStatus::SlotOnReceive(uint32_t recvCount)
{
    std::lock_guard<std::mutex> guard(m_lock);
    m_recvCount = recvCount;
    m_lastIoTime = ping_time_rate;
}

void NetStatus::SlotOnSend(uint32_t sendCount)
{
    std::lock_guard<std::mutex> guard(m_lock);
    m_sendCount = sendCount;
    m_lastIoTime = ping_time_rate;
}

