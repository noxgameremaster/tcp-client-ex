
#ifndef TASK_LARGE_FILE_H__
#define TASK_LARGE_FILE_H__

#include "abstractTask.h"

class NetPacket;
class IOFileStream;

class TaskLargeFile : public AbstractTask
{
private:
    std::unique_ptr<IOFileStream> m_file;
    uint32_t m_accumulate;

public:
    explicit TaskLargeFile(NetObject *parent);
    ~TaskLargeFile() override;

private:
    bool CreateDownloadFile(const std::string &url);
    void RequestChunkData();
    void ReportDownloadComplete();
    void LargeFileRequest(std::unique_ptr<NetPacket> &&reqPacket);
    void LargeFileGetChunk(std::unique_ptr<NetPacket> &&pack);
    void DoTask(std::unique_ptr<NetPacket> &&packet) override;
    std::string TaskName() const override;
};

#endif

