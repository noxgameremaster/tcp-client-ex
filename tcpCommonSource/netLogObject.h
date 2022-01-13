
#ifndef NET_LOG_OBJECT_H__
#define NET_LOG_OBJECT_H__

#include "netservice.h"
#include "printUtil.h"

class LoopThread;

class NetLogObject : public NetService
{
    using net_log_element = std::tuple<std::string, uint32_t>;
private:
    std::unique_ptr<LoopThread> m_logThread;
    std::list<net_log_element> m_netLog;

private:
    explicit NetLogObject();
    ~NetLogObject() override;

private:
    uint32_t GetLogRealColor(PrintUtil::ConsoleColor colr);
public:
    void AppendLogMessage(const std::string &message, PrintUtil::ConsoleColor colr = PrintUtil::ConsoleColor::COLOR_MIN);

private:
    bool IsNotEmpty() const;
    bool NavigateLogList();

    bool OnInitialize() override;
    bool OnStarted() override;
    void OnDeinitialize() override;
    void OnStopped() override;

public:
    static NetLogObject &LogObject();

private:
    mutable std::mutex m_lock;

public:
    DECLARE_SIGNAL(OnReleaseLogMessage, std::string, uint32_t)
};

#define NET_PUSH_LOGMSG(...)    \
    NetLogObject::LogObject().AppendLogMessage(__VA_ARGS__)

#endif

