
#include "netservice.h"
#include <ws2tcpip.h>

NetService::NetService(NetObject *parent)
    : NetObject(parent)
{ }

NetService::~NetService()
{ }

bool NetService::WsaStartup()
{
    static constexpr int wsa_no_error = 0;
    std::unique_ptr<WSAData> wsa(new WSADATA);

    if (WSAStartup(0x202, wsa.get()) == wsa_no_error)
    {
        m_wsaData = decltype(m_wsaData)(wsa.release(), [](WSAData *pWsa) { WSACleanup(); delete pWsa; });

        return true;
    }
    return false;
}

bool NetService::WsaCleanup()
{
    if (m_wsaData)
    {
        m_wsaData.reset();
        return true;
    }
    return false;
}

bool NetService::OnInitialize()
{
    return WsaStartup();
}

void NetService::OnDeinitialize()
{
    WsaCleanup();
}

bool NetService::OnStarted()
{
    return true;
}

void NetService::OnStopped()
{ }

bool NetService::Startup()
{
    return OnInitialize() ? OnStarted() : false;
}

void NetService::Shutdown()
{
    OnStopped();
    OnDeinitialize();
}

