
#ifndef IP_TO_BYTE_H__
#define IP_TO_BYTE_H__

#include <string>
#include <vector>
#include <list>

class IpToByte
{
private:
    std::string m_ipAddress;
    std::list<char> m_tokens;

private:
    int ReadC();
    void UnreadC(int c);
    int ReadNumeric(int c);

public:
    void SetIpAddress(const std::string &ipAddr);
    bool ToByteStream(std::vector<char> &dest);
};

#endif

