
#ifndef CORE_UI_H__
#define CORE_UI_H__

#include "ccobject.h"

#include <future>

class NetClient;
class IniFileMan;

class CoreUi : public CCObject
{
private:
    const std::string m_settingFileName;
    std::unique_ptr<NetClient> m_netMain;
    std::unique_ptr<IniFileMan> m_iniMan;
    std::future<bool> m_netRunner;

public:
    explicit CoreUi();
    ~CoreUi() override;

    void Initialize();
    void Deinitialize();

private:
    bool NetStartup();

public:
    void StartNetClient();

private:
    void ReceiveLogMessage(const std::string &msg, uint32_t colr);

public:
    void DoTestEcho();
    void DoTestFilePacket();

    void SendCommandToServer(const std::string &cmd);

    DECLARE_SIGNAL(OnForwardMessage, std::string, uint32_t)
};

#endif

