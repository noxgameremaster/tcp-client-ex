
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

const BinaryStream::value_type *BinaryStream::GetRaw()
{
    if (nullptr == m_context)
        return nullptr;

    return m_context->data();
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

void BinaryStream::SetBufferSize(const size_t resize)
{
    m_context->clear();
    m_context->resize(resize);
    m_seekpoint = 0;
}

