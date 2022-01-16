
#ifndef CORE_UI_H__
#define CORE_UI_H__

#include "netservice.h"
#include <future>

class NetClient;
class IniFileMan;
class NetPacket;
class DownloadFileInfo;

class CoreUi : public NetService
{
private:
    const std::string m_settingFileName;
    std::unique_ptr<NetClient> m_netMain;
    std::unique_ptr<IniFileMan> m_iniMan;
    std::future<bool> m_netRunner;

public:
    explicit CoreUi();
    ~CoreUi() override;

private:
    void OnInitialOnce() override;
    bool OnInitialize() override;
    bool OnStarted() override;
    void OnDeinitialize() override;
    void OnStopped() override;

private:
    bool NetInit();
    bool NetStartup();

public:
    void StartNetClient();

private:
    void ReceiveLogMessage(const std::string &msg, uint32_t colr);

public:
    void DoTestEcho();
    void DoTestFilePacket();

    void SendCommandToServer(const std::string &cmd);
    void StopCoreService();

private:
    void SlotGetInnerPacket(std::shared_ptr<NetPacket> &&packet);

public:
    DECLARE_SIGNAL(OnForwardMessage, std::string, uint32_t)
public:
    DECLARE_SIGNAL(OnSendInfoToFilePanel, std::shared_ptr<DownloadFileInfo>)
};

#endif

