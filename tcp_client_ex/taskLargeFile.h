
#ifndef TASK_LARGE_FILE_H__
#define TASK_LARGE_FILE_H__

#include "abstractTask.h"

class NetPacket;
class AsynchonousFileTask;

class TaskLargeFile : public AbstractTask
{
private:
    std::unique_ptr<AsynchonousFileTask> m_file;
    uint32_t m_accumulate;

public:
    explicit TaskLargeFile(NetObject *parent);
    ~TaskLargeFile() override;

private:
    void InnerSendFileInfo(const size_t amount);
    bool CreateDownloadFile(const std::string &url);
    void RequestChunkData();
    void ReportDownloadComplete();
    void LargeFileRequest(std::unique_ptr<NetPacket> &&reqPacket);
    void LargeFileGetChunk(std::unique_ptr<NetPacket> &&pack);
    void DoTask(std::unique_ptr<NetPacket> &&packet) override;
    std::string TaskName() const override;
    void SlotReportFileSeek(const size_t writeAmount);

private:
    DECLARE_SIGNAL(OnReportFileSeek, size_t)
};

#endif

