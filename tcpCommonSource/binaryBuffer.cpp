
#include "binaryBuffer.h"
#include <cassert>

BinaryBuffer::BinaryBuffer(const size_t &reserveSize)
    : BinaryStream(1)
{
    m_buffer.reserve(reserveSize);
    SetContext(&m_buffer);
}

BinaryBuffer::~BinaryBuffer()
{ }

bool BinaryBuffer::Append(const uint8_t src)
{
    if (m_buffer.capacity() <= m_buffer.size())
    {
        assert(false);
        return false;
    }

    m_buffer.push_back(src);
    return true;
}

void BinaryBuffer::Clear()
{
    if (m_buffer.size())
        m_buffer.clear();
    SetSeekpoint(0);
}

std::vector<uint8_t> BinaryBuffer::GetPart(const size_t &requestLength, const size_t &off)
{
    if (off >= m_buffer.size())
        return{ };
    
    std::vector<uint8_t>::const_iterator start = m_buffer.cbegin() + off;
    std::vector<uint8_t> res;
    size_t length = 0;

    while (start != m_buffer.cend())
    {
        if (length >= requestLength)
            break;

        ++length;
        res.push_back(*start);
        ++start;
    }
    return res;
}