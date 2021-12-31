
#include "netLogObject.h"
#include "loopThread.h"
#include "eventworker.h"

NetLogObject::NetLogObject()
    : NetService()
{
    m_logThread = std::make_unique<LoopThread>();

    m_logThread->SetTaskFunction([this]() { this->NavigateLogList(); });
    m_logThread->SetWaitCondition([this]() { return this->IsNotEmpty(); });
}

NetLogObject::~NetLogObject()
{ }

uint32_t NetLogObject::GetLogRealColor(PrintUtil::ConsoleColor colr)
{
    switch (colr)
    {
    case PrintUtil::ConsoleColor::COLOR_RED:
        return 0xff;
    case PrintUtil::ConsoleColor::COLOR_BLUE:
        return 0xff0000;
    case PrintUtil::ConsoleColor::COLOR_CYAN:
        return 0xffff80;
    case PrintUtil::ConsoleColor::COLOR_GREEN:
        return 0xff00;
    case PrintUtil::ConsoleColor::COLOR_PINK:
        return 0xff00ff;
    case PrintUtil::ConsoleColor::COLOR_VIOLET:
        return 0xff0080;
    case PrintUtil::ConsoleColor::COLOR_YELLOW:
        return 0xffff;
    case PrintUtil::ConsoleColor::COLOR_LIGHTGREEN:
        return 181 | (230 << 8) | (29 << 16);
    default:
        return 0;
    }
}

void NetLogObject::AppendLogMessage(const std::string &message, PrintUtil::ConsoleColor colr)
{
    uint32_t realColor = GetLogRealColor(colr);
    std::string pushMsg = message;

    {
        std::lock_guard<std::mutex> guard(m_lock);

        m_netLog.emplace_back(std::move(pushMsg), realColor);
    }
    m_logThread->Notify();
}

bool NetLogObject::IsNotEmpty() const
{
    std::lock_guard<std::mutex> guard(m_lock);

    return !m_netLog.empty();
}

void NetLogObject::NavigateLogList()
{
    {
        std::lock_guard<std::mutex> guard(m_lock);

        for (const auto &elem : m_netLog)
            EventWorker::Instance().AppendTask(&m_OnReleaseLogMessage, std::get<0>(elem), std::get<1>(elem));

        m_netLog.clear();
    }
}

bool NetLogObject::OnInitialize()
{
    return true;
}

bool NetLogObject::OnStarted()
{
    return m_logThread->Startup();
}

void NetLogObject::OnDeinitialize()
{ }

void NetLogObject::OnStopped()
{
    m_logThread->Shutdown();
}

NetLogObject &NetLogObject::LogObject()
{
    static NetLogObject log;

    return log;
}

