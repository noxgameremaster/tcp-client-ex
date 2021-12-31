
#ifndef CORE_UI_H__
#define CORE_UI_H__

#include "ccobject.h"

class NetClient;

class CoreUi : public CCObject
{
private:
    std::unique_ptr<NetClient> m_netMain;

public:
    explicit CoreUi();
    ~CoreUi() override;

    void Initialize();
    void Deinitialize();
    bool StartNetClient();

private:
    void ReceiveLogMessage(const std::string &msg, uint32_t colr);

public:
    void DoTestEcho();
    void DoTestFilePacket();

    DECLARE_SIGNAL(OnForwardMessage, std::string, uint32_t)
};

#endif

