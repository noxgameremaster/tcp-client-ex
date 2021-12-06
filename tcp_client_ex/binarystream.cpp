
#include "binarystream.h"

BinaryStream::BinaryStream(const size_t reserve_size)
{
    m_binary.reserve(reserve_size);
    m_seekpoint = 0;
    m_context = &m_binary;
}

BinaryStream::~BinaryStream()
{
}

bool BinaryStream::PutStreamRaw(const value_type *raw, size_t length)
{
    if (m_context->capacity() < length)
        return false;

    m_context->resize(length);
    std::copy_n(raw, length, m_context->begin());
    return true;
}

void BinaryStream::SetSeekpoint(uint32_t pos)
{
    if (m_context->empty())
    {
        m_seekpoint = 0;
        return;
    }

    if (pos < m_context->size())
        m_seekpoint = pos;
}

void BinaryStream::BufferResize(const size_t buffersize)
{
    m_context->clear();
    if (m_context->size() != buffersize)
        m_context->resize(buffersize);

    m_seekpoint = 0;
}