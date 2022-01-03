
#ifndef PARTITION_POOL_H__
#define PARTITION_POOL_H__

#include "netobject.h"
#include <stack>
#include <vector>

class PartitionPool : public NetObject
{
    class Partition;
private:
    std::stack<int> m_stack;
    std::vector<Partition> m_mempool;

public:
    explicit PartitionPool(size_t count = 0);
    virtual ~PartitionPool();

    PartitionPool(const PartitionPool &) = delete;
    PartitionPool &operator=(const PartitionPool &) = delete;

protected:
    void Resize(size_t count);

private:
    bool Pop(int &dest);
    Partition *GetPart(int index);
    bool CheckCapacity(size_t ssize);

protected:
    bool GetTotalSize(const int &index, size_t &destsize);
    bool PushData(const uint8_t *stream, const size_t ssize, int &getindex);
    bool GetData(const int &index, uint8_t *destStream);
    void ReturnData(const int &index);
};

#endif

