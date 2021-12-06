
#include "partitionpool.h"
#include <stdexcept>

class PartitionPool::Partition
{
    friend PartitionPool;

private:
    static constexpr size_t space_size = 64;

    size_t m_totalsize;
    size_t m_size;
    uint8_t m_data[space_size];
    Partition *m_nextpart;
    int m_index;

public:
    explicit Partition()
    {
        m_size = 0;
        m_nextpart = nullptr;
        m_index = 0;
        memset(m_data, 0, space_size);
    }
};

PartitionPool::PartitionPool(size_t count)
{
    Resize(count);
}

PartitionPool::~PartitionPool()
{ }

//call once
void PartitionPool::Resize(size_t count)
{
    if (!m_mempool.empty())     //todo. 크기가 정해지면, 이후부터는 변경금지. 추후 크기를 템플릿 화
        return;

    if (!count)
        return;

    m_mempool.resize(count);
    while (count --)
        m_stack.push(static_cast<int>(count));
}

bool PartitionPool::Pop(int &dest)
{
    if (m_stack.empty())
        return false;

    dest = m_stack.top();
    m_stack.pop();
    return true;
}

PartitionPool::Partition *PartitionPool::GetPart(int index)
{
    try
    {
        Partition &mempool = m_mempool.at(index);
        return &mempool;
    }
    catch (const std::out_of_range &)
    {
        return nullptr;
    }
}

bool PartitionPool::CheckCapacity(size_t ssize)
{
    size_t rsize = m_stack.size() * Partition::space_size;
    return ssize < rsize;
}

bool PartitionPool::GetTotalSize(const int &index, size_t &destsize)
{
    Partition *part = GetPart(index);

    if (part == nullptr)
        return false;

    destsize = part->m_totalsize;
    return true;
}

bool PartitionPool::PushData(uint8_t *stream, const size_t ssize, int &getindex)
{
    if (!CheckCapacity(ssize))
        return false;

    int index = 0;
    int peek = m_stack.top();
    Partition *prevPart = nullptr, *peeknode = GetPart(peek);
    size_t streamOff = 0, csize = ssize;

    while (csize)
    {
        if (!Pop(index))
            return false;

        Partition *part = GetPart(index);

        part->m_index = index;
        part->m_nextpart = nullptr;
        if (csize < Partition::space_size)
        {
            part->m_size = csize;
            csize = 0;
        }
        else
        {
            part->m_size = Partition::space_size;
            csize -= Partition::space_size;
        }
        memcpy_s(part->m_data, part->m_size, stream + streamOff, part->m_size);
        streamOff += part->m_size;
        if (prevPart != nullptr)
            prevPart->m_nextpart = part;
        prevPart = part;
    }
    getindex = peek;
    peeknode->m_totalsize = ssize;
    return true;
}

bool PartitionPool::GetData(const int &index, uint8_t *destStream)
{
    if (destStream == nullptr)
        return false;

    Partition *part = GetPart(index);
    size_t streamPos = 0;

    while (part != nullptr)
    {
        memcpy_s(destStream + streamPos, part->m_size, part->m_data, part->m_size);
        streamPos += part->m_size;
        part = part->m_nextpart;
    }
    return true;
}

void PartitionPool::ReturnData(const int &index)
{
    Partition *part = GetPart(index);

    while (part != nullptr)
    {
        m_stack.push(part->m_index);
        part = part->m_nextpart;
    }
}



