
#include "netStatus.h"
#include "loopThread.h"
#include "eventworker.h"

NetStatus::NetStatus(NetObject *parent)
    : NetService(parent)
{
    m_recvCount = 0;
    m_sendCount = 0;
    m_lastIoTime = 0;

    m_parent = parent;
}

NetStatus::~NetStatus()
{ }

bool NetStatus::CheckNetStatus()
{
    static decltype(m_recvCount) s_recvCount = 0;
    static decltype(m_sendCount) s_sendCount = 0;

    {
        std::lock_guard<std::mutex> guard(m_lock);
        if (m_lastIoTime)
        {
            --m_lastIoTime;
        }
        else
        {
            m_lastIoTime = ping_time_rate;

            do
            {
                if (m_recvCount == s_recvCount && m_sendCount == s_sendCount)
                    break;

                if (m_recvCount != s_recvCount)
                    s_recvCount = m_recvCount;
                if (m_sendCount != s_sendCount)
                    s_sendCount = m_sendCount;
                QUEUE_EMIT(m_OnReportPing, m_recvCount, m_sendCount);
            }
            while (false);
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    return true;
}

bool NetStatus::OnInitialize()
{
    if (m_statusThread)
        return false;

    m_statusThread = std::make_unique<LoopThread>(this);
    m_statusThread->SetTaskFunction([this]() { return this->CheckNetStatus(); });

    return m_statusThread->Startup();
}

void NetStatus::OnDeinitialize()
{
    m_statusThread->Shutdown();
    m_statusThread.reset();
}

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

