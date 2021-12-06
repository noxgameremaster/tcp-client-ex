
#include "iptobyte.h"
#include <algorithm>
#include <iterator>
#include <sstream>

static constexpr int stream_eof = -1;

int IpToByte::ReadC()
{
    if (m_tokens.empty())
        return stream_eof;

    int c = m_tokens.front();

    m_tokens.pop_front();
    return c;
}

void IpToByte::UnreadC(int c)
{
    m_tokens.push_front(static_cast<std::remove_reference<decltype(m_tokens.front())>::type>(c));
}

int IpToByte::ReadNumeric(int c)
{
    std::vector<char> buffer(1, static_cast<char>(c));

    do
    {
        int n = ReadC();

        if (n == stream_eof || !isdigit(n))
        {
            UnreadC(n);
            break;
        }
        buffer.push_back(static_cast<char>(n));
    }
    while (true);

    int result = 0;
    std::stringstream ss(buffer.data());

    ss >> result;

    return result;
}

void IpToByte::SetIpAddress(const std::string &ipAddr)
{
    if (!m_tokens.empty())
        m_tokens.clear();

    std::copy(ipAddr.begin(), ipAddr.end(), std::insert_iterator<decltype(m_tokens)>(m_tokens, m_tokens.end()));
}

bool IpToByte::ToByteStream(std::vector<char> &dest)
{
    do
    {
        int c = ReadC();

        switch (c)
        {
        case stream_eof:
            return true;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        {
            int ivalue = ReadNumeric(c);

            if (ivalue & (~0xff))
                return false;
            dest.push_back(static_cast<char>(ivalue));
        }
        break;
        case '.':
            break;
        default:    //error. unknown token
            return false;
        }
    }
    while (true);
}
