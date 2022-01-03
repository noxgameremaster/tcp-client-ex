
#include "pch.h"
#include "coreui.h"
#include "netclient.h"
#include "netLogObject.h"
#include "eventworker.h"

CoreUi::CoreUi()
    : CCObject()
{ }

CoreUi::~CoreUi()
{ }

void CoreUi::Initialize()
{
    EventWorker::Instance().Start();
    NetLogObject::LogObject().Startup();

    NetLogObject::LogObject().OnReleaseLogMessage().Connection(&CoreUi::ReceiveLogMessage, this);

    m_netMain = std::make_unique<NetClient>();
}

void CoreUi::Deinitialize()
{
    m_netMain->Shutdown();

    NetLogObject::LogObject().Shutdown();
    EventWorker::Instance().Stop();
}

bool CoreUi::StartNetClient()
{
    if (m_netMain->Startup())
        return true;

    m_OnForwardMessage.Emit("fail...", 0xff);
    return false;
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
    if (m_netMain)
        m_netMain->ClientTestSendFileRequest("C:\\Users\\인스유틸\\source\\repos\\tcp_client_ex\\tcp_client_ex\\serverfile\\maple.map");
}

void CoreUi::SendCommandToServer(const std::string &cmd)
{
    if (m_netMain)
        m_netMain->ClientSendChat(cmd);
}