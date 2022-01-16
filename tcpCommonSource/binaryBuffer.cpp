
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

bool BinaryBuffer::AppendImpl(const uint8_t *streamPtr, const size_t length)
{
    if (m_buffer.capacity() <= m_buffer.size() + length)
    {
        assert(false);
        return false;
    }
    uint8_t *destPtr = &m_buffer.back()+1;
    
    m_buffer.resize(m_buffer.size() + length);  //WARNING. must be reserved enough size!!
    memcpy_s(destPtr, length, streamPtr, length);
    return true;
}

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

std::vector<uint8_t> BinaryBuffer::GetPart(const size_t &requestLength, const size_t &off)  //여기에서 소요시간 많음
{
    if (off >= m_buffer.size())
        return{ };
    
    std::vector<uint8_t>::const_iterator start = m_buffer.cbegin() + off;
    static std::vector<uint8_t> res;
    size_t length = 0;

    res.clear();
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

std::vector<uint8_t> BinaryBuffer::GetPartRe(const size_t &requestLength, const size_t &offset)
{
    if (offset < m_buffer.size())
    {
        const size_t realSize = ((offset + requestLength) > m_buffer.size()) ? ((offset + requestLength) - m_buffer.size()) : requestLength;
        std::vector<uint8_t> output(realSize);
        std::copy(m_buffer.cbegin() + offset, m_buffer.cbegin() + realSize, output.begin());
        return output;
    }
    return{ };
}