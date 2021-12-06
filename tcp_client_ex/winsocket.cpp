
#include "winsocket.h"
#include "iptobyte.h"
#include <ws2tcpip.h>
#include <vector>

WinSocket::WinSocket(socket_type sock)
{
    if (sock != INVALID_SOCKET)
        m_socket = decltype(m_socket)(new socket_type(sock), [](socket_type *s) { closesocket(*s); delete s; });

    m_ipAddr = "127.0.0.1";
    m_portId = 18590;
}

WinSocket::~WinSocket()
{ }

bool WinSocket::IsValid()
{
    if (!m_socket)
        return false;

    return *m_socket != INVALID_SOCKET;
}

bool WinSocket::CreateSocket()
{
    if (m_socket)
        return true;

    std::unique_ptr<socket_type> winsock(new socket_type(socket(AF_INET, SOCK_STREAM, 0)));

    if (*winsock == INVALID_SOCKET)
        return false;

    m_socket = decltype(m_socket)(winsock.release(), [](socket_type *sock) { closesocket(*sock); delete sock; });

    return true;
}

bool WinSocket::MakeHint()
{
    std::unique_ptr<sockaddr_in> nethint(new sockaddr_in);

    nethint->sin_family = AF_INET;
    nethint->sin_port = SwapEndian(m_portId);

    std::vector<char> dest;
    IpToByte tobyte;

    tobyte.SetIpAddress(m_ipAddr);
    if (!tobyte.ToByteStream(dest))
        return false;

    char *pSinAddr = reinterpret_cast<char *>(&nethint->sin_addr);        //Unsafe. !FIXME!

    std::copy(dest.begin(), dest.end(), pSinAddr);
    m_netHint = std::move(nethint);

    return true;
}

bool WinSocket::SetSocketOpt(const std::string &ipAddress, uint16_t portNumber)
{
    if (ipAddress.empty() || !portNumber)
        return false;

    m_ipAddr = ipAddress;
    m_portId = portNumber;
    return MakeHint();
}

socket_type WinSocket::GetFd()
{
    if (!m_socket)
        return INVALID_SOCKET;

    return *m_socket;
}

bool WinSocket::ReceiveImpl(char *buff, const size_t length, int &readbytes)
{
    readbytes = recv(*m_socket, buff, length, 0);

    if (readbytes <= 0)
        return false;

    return true;
}

bool WinSocket::SendImpl(const char *buffer, const size_t length, int &sendbytes)
{
    sendbytes = send(*m_socket, buffer, length, 0);

    return (sendbytes <= 0);
}

bool WinSocket::SendAll(const char *buffer, const size_t length)
{
    int sendbytes = 0;
    size_t pos = 0;

    while (pos < length)
    {
        if (!SendImpl(buffer + pos, length - pos, sendbytes))
            return false;

        pos += sendbytes;
    }
    return true;
}

bool WinSocket::SendAll(const uint8_t *ubuffer, const size_t length)
{
    return SendAll(reinterpret_cast<const char *>(ubuffer), length);
}

bool WinSocket::Connect()
{
    if (!MakeHint())
        return false;

    if (*m_socket == INVALID_SOCKET)
        return false;

    return SOCKET_ERROR != connect(*m_socket, reinterpret_cast<const sockaddr *>(m_netHint.get()), sizeof(*m_netHint));
}

bool WinSocket::Bind()
{
    if (!MakeHint())
        return false;

    return SOCKET_ERROR != bind(*m_socket, reinterpret_cast<const sockaddr *>(m_netHint.get()), sizeof(*m_netHint));
}

bool WinSocket::Listen()
{
    if (!m_netHint)
        return false;

    return SOCKET_ERROR != listen(*m_socket, SOMAXCONN);
}

