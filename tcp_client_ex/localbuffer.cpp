
#include "localbuffer.h"

LocalBuffer::LocalBuffer()
    : BinaryStream(1)
{
    m_seekpoint = 0;
    m_buffer = decltype(m_buffer)(local_buffer_reserve, 0);

    SetContext(&m_buffer);
}

LocalBuffer::~LocalBuffer()
{ }

bool LocalBuffer::IsEmpty() const
{
    return m_seekpoint == 0;
}

bool LocalBuffer::CheckRemaining(size_t length)
{
    size_t remainAmount = local_buffer_reserve - m_seekpoint;

    return static_cast<int>(remainAmount - length) >= 0;    //fit to fully
}

bool LocalBuffer::Append(const uint8_t *src, size_t length)
{
    if (!CheckRemaining(length))
        return false;

    std::copy_n(src, length, &m_buffer[m_seekpoint]);
    m_seekpoint += length;

    return true;
}

void LocalBuffer::Clear()
{
    if (!m_seekpoint)
        return;

    m_seekpoint = 0;
}

void LocalBuffer::Pull(const size_t &count)
{
    if (!m_seekpoint || !count)
        return;

    if (count >= m_seekpoint)
    {
        Clear();
        return;
    }

    std::copy_n(&m_buffer[count], m_seekpoint - count, m_buffer.begin());
    m_seekpoint -= count;
}



