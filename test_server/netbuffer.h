
#ifndef NET_BUFFER_H__
#define NET_BUFFER_H__

#include <vector>

class NetBuffer
{
private:
    std::vector<uint8_t> m_buffer;

public:
    NetBuffer();
    ~NetBuffer();

private:
    bool CheckCapacity(const size_t &size);

public:
    bool Append(const uint8_t *stream, const size_t &length);

    template <class Container>
    bool Append(const Container &src)
    {
        if (!CheckCapacity(src.size()))
            return false;

        typename Container::iterator it = src.end();
        m_buffer.resize(m_buffer.size() + src.size());
        std::copy(src.begin(), src.end(), it);
        return true;
    }
};

#endif
