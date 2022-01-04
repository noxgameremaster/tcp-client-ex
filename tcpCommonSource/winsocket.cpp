
#include "winsocket.h"
#include "iptobyte.h"
#include <ws2tcpip.h>
#include <vector>
#include <iterator>

#pragma warning(disable:4996)

WinSocket::WinSocket(socket_type sock)
{
    if (sock != INVALID_SOCKET)
        m_socket = decltype(m_socket)(new socket_type(sock), [](socket_type *s) { closesocket(*s); delete s; });
}

WinSocket::~WinSocket()
{ }

WinSocket &WinSocket::operator=(WinSocket &&movSocket) noexcept
{
    ShareOption(&movSocket);
    if (movSocket.m_netHint)
        m_netHint = std::move(movSocket.m_netHint);
    if (movSocket.m_socket)
        m_socket = std::move(movSocket.m_socket);

    return *this;
}

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
    nethint->sin_port = SwapEndian(GetPortNumber());

    std::vector<char> dest;
    IpToByte tobyte;

    tobyte.SetIpAddress(GetIpAddress());
    if (!tobyte.ToByteStream(dest))
        return false;

    char *pSinAddr = reinterpret_cast<char *>(&nethint->sin_addr);        //Unsafe. !FIXME!

    std::copy(dest.begin(), dest.end(), stdext::checked_array_iterator<char *>(pSinAddr, 4));
    m_netHint = std::move(nethint);

    return true;
}

bool WinSocket::SetSocketOpt(const std::string &ipAddress, uint16_t portNumber)
{
    if (ipAddress.empty() || !portNumber)
        return false;

    SetNetOption(ipAddress, portNumber);
    return MakeHint();
}

socket_type WinSocket::GetFd()
{
    if (!m_socket)
        return INVALID_SOCKET;

    return *m_socket;
}

bool WinSocket::ReceiveImpl(uint8_t *buff, const size_t length, int &readbytes)
{
    readbytes = recv(*m_socket, reinterpret_cast<char *>(buff), length, 0);

    OutputDebugString("server receive\n");
    if (readbytes <= 0)
        return false;

    return true;
}

bool WinSocket::SendImpl(const char *buffer, const size_t length, int &sendbytes)
{
    OutputDebugString("server sent\n");
    sendbytes = send(*m_socket, buffer, length, 0);

    return sendbytes != SOCKET_ERROR;
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

bool WinSocket::Accept(WinSocket &&dest)
{
    socket_type s = accept(*m_socket, nullptr, nullptr);

    if (INVALID_SOCKET == s)
        return false;

    dest = WinSocket(s);
    return true;
}

bool WinSocket::operator==(const WinSocket &other)
{
    if (!m_socket)
        return false;
    if (INVALID_SOCKET == *m_socket)
        return false;
    if (!other.m_socket)
        return false;

    return *m_socket == *other.m_socket;
}