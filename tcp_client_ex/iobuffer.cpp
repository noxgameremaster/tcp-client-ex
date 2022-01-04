#include "iobuffer.h"
//#include "localbuffer.h"
#include <functional>
#include <memory>

IOBuffer::IOBuffer()
    : PartitionPool(receive_buffer_max_size)
{
    m_largebufferRealsize = 0;
}

IOBuffer::~IOBuffer()
{
}

bool IOBuffer::PopIndex(int &destIndex)
{
    if (m_indexes.empty())
        return false;

    destIndex = m_indexes.front();
    m_indexes.pop_front();
    return true;
}

bool IOBuffer::StoreToLargebuffer(const int &srcIndex)
{
    size_t totalsize = 0;

    if (!GetTotalSize(srcIndex, totalsize))
        return false;

    if (m_largeBuffer.size() < totalsize)
        return false;

    if (!GetData(srcIndex, m_largeBuffer.data()))
        return false;

    m_largebufferRealsize = totalsize;
    return true;
}

void IOBuffer::TriggeredWhenPush()
{
    decltype(m_triggers.cbegin()) triggerIterator = m_triggers.cbegin();
    std::list<decltype(triggerIterator)> delList;

    while (triggerIterator != m_triggers.cend())
    {
        if (!(*triggerIterator)())
            delList.push_back(triggerIterator);
        ++triggerIterator;
    }
    for (decltype(triggerIterator) &it : delList)
        m_triggers.erase(it);
}

bool IOBuffer::PushBuffer(const uint8_t *buffer, size_t bufferSize)
{
    int getindex = 0;

    {
        std::lock_guard<std::mutex> gaurd(m_lock);

        if (!PushData(buffer, bufferSize, getindex))
            return false;

        m_indexes.push_back(getindex);
    }
    TriggeredWhenPush();
    return true;
}

bool IOBuffer::PopBufferAlloc(std::unique_ptr<uint8_t[]> &&destptr, size_t &bufferSize)
{
    int index = 0;
    size_t totalsize = 0;
    std::unique_ptr<uint8_t[]> alloc;

    {
        std::lock_guard<std::mutex> guard(m_lock);

        if (!PopIndex(index))
            return false;

        {
            std::unique_ptr<int, std::function<void(int *)>> retIndex(&index, [this](const int *ret)
            { ReturnData(*ret); });

            if (!GetTotalSize(index, totalsize))
                return false;

            alloc = std::unique_ptr<uint8_t[]>(new uint8_t[totalsize]);

            if (!GetData(index, alloc.get()))
                return false;

            destptr = std::move(alloc);
            bufferSize = totalsize;
        }
    }
    return true;
}

bool IOBuffer::SetLargeBufferScale(size_t scale)
{
    if (m_largeBuffer.size())
        return false;

    m_largeBuffer.resize(scale);
    return true;
}

bool IOBuffer::IsEmpty() const
{
    std::lock_guard<std::mutex> guard(m_lock);

    return m_indexes.empty();
}

bool IOBuffer::SetTrigger(NetObject *trigger, std::function<void()> &&fn)
{
    std::weak_ptr<NetObjectImpl> alive;
    GetImpl(trigger, alive);

    if (alive.expired())
        return false;

    m_triggers.push_back(
        [r = alive, fn = std::move(fn)]
    ()
    {
        if (r.expired())
            return false;
        fn();
        return true;
    }
    );
    return true;
}

//void IOBuffer::MoveBuffer(std::shared_ptr<LocalBuffer> localbuffer)
//{
//    size_t readsize = 0;
//
//    while (true)
//    {
//        {
//            std::unique_ptr<uint8_t[]> alloc;
//
//            if (!PopBufferAlloc(std::move(alloc), readsize))
//                break;
//
//            if (!localbuffer->Append(alloc.get(), readsize))
//                break;
//        }
//    }
//}


