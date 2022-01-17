
#include "pch.h"
#include "coreui.h"
#include "netclient.h"
#include "filepacket.h"
#include "netLogObject.h"
#include "downloadFileInfo.h"
#include "eventworker.h"
#include "iniFileMan.h"
#include "stringHelper.h"

using namespace _StringHelper;

CoreUi::CoreUi()
    : NetService(),
    m_settingFileName("appsetting.txt")
{ }

CoreUi::~CoreUi()
{ }

void CoreUi::OnInitialOnce()
{
    EventWorker::Instance();
    NetLogObject::LogObject().Startup();
    NetLogObject::LogObject().OnReleaseLogMessage().Connection(&CoreUi::ReceiveLogMessage, this);

    m_netMain = std::make_unique<NetClient>();

    std::weak_ptr<NetObjectImpl> ref;

    GetImpl(ref);

    m_netMain->RegistInnerPacketListener(this, [this, ref = std::move(ref)](std::shared_ptr<NetPacket> &&packet)
    {
        if (ref.expired())
            return;

        this->SlotGetInnerPacket(std::move(packet));
    });
}

bool CoreUi::OnInitialize()
{
    m_iniMan = std::make_unique<IniFileMan>();

    return true;
}

bool CoreUi::OnStarted()
{
    return true;
}

void CoreUi::OnDeinitialize()
{
    //m_netRunner.get();  ///here error //FIXME.
    m_netMain->Shutdown();
}

void CoreUi::OnStopped()
{ }

bool CoreUi::NetInit()
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

    return true;
}

bool CoreUi::NetStartup()
{
    std::string connectInfo = stringFormat("ip: %s, port: %d", m_netMain->GetIpAddress(), static_cast<int>(m_netMain->GetPortNumber()));

    m_OnForwardMessage.Emit(connectInfo, 0xff);
    if (!m_netMain->Startup())
    {
        m_OnForwardMessage.Emit("fail to connect...", 0xff);
        return false;
    }
    return true;
}

void CoreUi::StartNetClient()
{
    if (NetInit())
    {
        m_netRunner = std::async([this]() { return this->NetStartup(); });
    }
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

void CoreUi::StopCoreService()
{
    NetLogObject::LogObject().Shutdown();
    //EventWorker::Instance().Stop();
}

void CoreUi::SlotGetInnerPacket(std::shared_ptr<NetPacket> &&packet)
{
    std::string packetName = packet->ClassName();

    if (packetName == "FilePacket")
    {
        auto filePacket = dynamic_cast<FilePacket *>(packet.get());
        std::shared_ptr<DownloadFileInfo> fileInfo(new DownloadFileInfo);

        fileInfo->SetFileInfo(filePacket->GetFileName(), filePacket->GetFilePath(), filePacket->GetFilesize(), filePacket->GetDownloadBytes());
        QUEUE_EMIT(m_OnSendInfoToFilePanel, std::move(fileInfo));
        return;
    }
    NET_PUSH_LOGMSG(stringFormat("get packet %s", packet->ClassName()));
}

