
#include "netservice.h"
#include <ws2tcpip.h>

NetService::NetService(NetObject *parent)
    : NetObject(parent)
{
    m_turnOn = false;
    m_onceInvokable = [this]() { this->m_onceInvokable = { }; this->OnInitialOnce(); };
}

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

std::string NetService::GetErrorMessage()
{
    int iError = GetLastError();
    char *pError = nullptr;

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        nullptr,
        iError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&pError,
        0,
        nullptr);

    //CLogManager *pCLogManager = CSingleton::pCLogManager;

    //pCLogManager->WriteLog("Error : %d : %s", iError, pError);
    //Beep(1000, 1000);
    std::string errorString = pError;
    OutputDebugString(pError);

    LocalFree(pError);
    //ASSERT(0);

    return errorString;
}

void NetService::NotifyOccurError(NetService *net, const std::string &errorTitle, const std::string &errorMessage)
{
    if (net != nullptr)
        net->OnError(errorTitle, errorMessage);
}

bool NetService::Startup()
{
    {
        std::unique_lock<std::mutex> localLock(m_lock);
        if (m_turnOn)
            return false;
        m_onceInvokable();

        m_turnOn = OnInitialize() ? OnStarted() : false;
    }

    return m_turnOn;
}

void NetService::Shutdown()
{
    {
        std::unique_lock<std::mutex> localLock(m_lock);
        if (!m_turnOn)
            return;

        m_turnOn = false;
    }
    OnStopped();
    OnDeinitialize();
}

