
#include "pch.h"
#include "coreui.h"
#include "netclient.h"
#include "netLogObject.h"
#include "eventworker.h"
#include "iniFileMan.h"
#include "stringHelper.h"

using namespace _StringHelper;

CoreUi::CoreUi()
    : CCObject(),
    m_settingFileName("appsetting.txt")
{ }

CoreUi::~CoreUi()
{ }

void CoreUi::Initialize()
{
    EventWorker::Instance().Start();
    NetLogObject::LogObject().Startup();

    NetLogObject::LogObject().OnReleaseLogMessage().Connection(&CoreUi::ReceiveLogMessage, this);

    m_netMain = std::make_unique<NetClient>();
    m_iniMan = std::make_unique<IniFileMan>();
}

void CoreUi::Deinitialize()
{
    m_netRunner.get();
    m_netMain->Shutdown();

    NetLogObject::LogObject().Shutdown();
    EventWorker::Instance().Stop();
}

bool CoreUi::NetStartup()
{
    if (!m_iniMan->ReadIni(m_settingFileName))
    {
        std::string errmsg = stringFormat("i couldn't find a file called '%s'", m_settingFileName);

        m_OnForwardMessage.Emit(errmsg, 0xff);
        return false;
    }

    std::string ip, port;

    m_iniMan->GetItemValue("Network", "ip_address", ip);
    m_iniMan->GetItemValue("Network", "port_number", port);

    if (!m_netMain->SetNetworkParam(ip, port))
    {
        std::string errmsg = stringFormat("wrong parameter! %s or %s", ip, port);

        m_OnForwardMessage.Emit(errmsg, 0xff);
        return false;
    }

    if (m_netMain->Startup())
        return true;

    m_OnForwardMessage.Emit("fail to connect...", 0xff);
    return false;
}

void CoreUi::StartNetClient()
{
    m_netRunner = std::async([this]() { return this->NetStartup(); });

    
}

void CoreUi::ReceiveLogMessage(const std::string &msg, uint32_t colr)
{
    m_OnForwardMessage.Emit(msg, colr);
}

void CoreUi::DoTestEcho()
{
    if (m_netMain)
        m_netMain->ClientSendEcho();
}

void CoreUi::DoTestFilePacket()
{
    if (!m_iniMan)
        return;

    std::string reqUrl;

    if (!m_iniMan->GetItemValue("File", "file_url", reqUrl))
    {
        m_OnForwardMessage.Emit("the ini file has no file_url key in file section", 0xff);
        return;
    }

    if (m_netMain)
        m_netMain->ClientTestSendFileRequest(reqUrl);
}

void CoreUi::SendCommandToServer(const std::string &cmd)
{
    if (m_netMain)
        m_netMain->ClientSendChat(cmd);
}