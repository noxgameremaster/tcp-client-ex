
#include "netbuffer.h"

NetBuffer::NetBuffer()
{
    m_buffer.reserve(0x1000);
}

NetBuffer::~NetBuffer()
{ }

bool NetBuffer::CheckCapacity(const size_t &size)
{
    if (!size)
        return false;

    return (size < m_buffer.capacity());
}

bool NetBuffer::Append(const uint8_t *stream, const size_t &length)
{
    size_t cur = 0;

    while (cur < length)
        m_buffer.push_back(stream[cur++]);

    return true;
}

