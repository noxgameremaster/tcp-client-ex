
#include "netLogObject.h"
#include "loopThread.h"
#include "eventworker.h"

template <uint8_t R, uint8_t G, uint8_t B>
struct RgbToUInt
{
    static constexpr uint32_t rgbUint = static_cast<uint32_t>(R | (G << 8) | (B << 16));
};

NetLogObject::NetLogObject()
    : NetService()
{
    m_logThread = std::make_unique<LoopThread>();

    m_logThread->SetTaskFunction([this]() { return this->NavigateLogList(); });
    m_logThread->SetWaitCondition([this]() { return this->IsNotEmpty(); });
}

NetLogObject::~NetLogObject()
{ }

uint32_t NetLogObject::GetLogRealColor(PrintUtil::ConsoleColor colr)
{
    switch (colr)
    {
    case PrintUtil::ConsoleColor::COLOR_RED: return RgbToUInt<255, 0, 0>::rgbUint;
    case PrintUtil::ConsoleColor::COLOR_BLUE: return RgbToUInt<0, 0, 255>::rgbUint;
    case PrintUtil::ConsoleColor::COLOR_CYAN: return RgbToUInt<128, 255, 255>::rgbUint;
    case PrintUtil::ConsoleColor::COLOR_GREEN: return RgbToUInt<0, 255, 0>::rgbUint;
    case PrintUtil::ConsoleColor::COLOR_PINK: return RgbToUInt<255, 0, 255>::rgbUint;
    case PrintUtil::ConsoleColor::COLOR_VIOLET: return RgbToUInt<181, 89, 181>::rgbUint;
    case PrintUtil::ConsoleColor::COLOR_YELLOW: return RgbToUInt<228, 220, 10>::rgbUint;
    case PrintUtil::ConsoleColor::COLOR_LIGHTGREEN: return RgbToUInt<0, 255, 128>::rgbUint;
    case PrintUtil::ConsoleColor::COLOR_DARKRED: return RgbToUInt<191, 40, 45>::rgbUint;
    case PrintUtil::ConsoleColor::COLOR_GREY: return RgbToUInt<128, 131, 160>::rgbUint;
    case PrintUtil::ConsoleColor::COLOR_EMERALD: return RgbToUInt<64, 128, 140>::rgbUint;
    case PrintUtil::ConsoleColor::COLOR_WHITE: return RgbToUInt<238, 255, 249>::rgbUint;
    case PrintUtil::ConsoleColor::COLOR_DARKBLUE: return RgbToUInt<11, 84, 140>::rgbUint;
    case PrintUtil::ConsoleColor::COLOR_DARKWHITE: return RgbToUInt<198, 192, 164>::rgbUint;
    case PrintUtil::ConsoleColor::COLOR_DARKYELLOW: return RgbToUInt<255, 128, 64>::rgbUint;
    default: return 0;
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

bool NetLogObject::NavigateLogList()
{
    {
        std::lock_guard<std::mutex> guard(m_lock);

        for (const auto &elem : m_netLog)
            EventWorker::Instance().AppendTask(&m_OnReleaseLogMessage, std::get<0>(elem), std::get<1>(elem));

        m_netLog.clear();
    }
    return true;
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

