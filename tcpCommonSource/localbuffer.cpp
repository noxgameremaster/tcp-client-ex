#include "localbuffer.h"
#include <iterator>

#pragma warning(disable:4996)

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

    std::copy_n(src, length, stdext::checked_array_iterator<uint8_t *>(m_buffer.data(), m_buffer.size(), m_seekpoint));
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

std::vector<uint8_t> LocalBuffer::GetPartStream(const size_t &off, const size_t &length)
{
    if (off > m_seekpoint)
        return {};

    size_t lastpos = off + length;
    
    if (lastpos > m_seekpoint)
        lastpos = m_seekpoint;

    return std::vector<uint8_t>(m_buffer.cbegin() + off, m_buffer.cbegin() + lastpos);
}