
#ifndef TASK_FILE_STREAM_H__
#define TASK_FILE_STREAM_H__

#include "abstracttask.h"
#include <vector>

class LargeFile;
class RunClock;

class TaskFileStream : public AbstractTask
{
private:
    std::string m_filename;
    std::string m_pathname;
    size_t m_filesize;

    uint8_t m_error;

    std::unique_ptr<RunClock> m_timeCounter;

    std::string m_uploadPath;

public:
    TaskFileStream(NetObject *parent);
    ~TaskFileStream();

private:
    void WriteFileMetaData();
    void ReportFileMetaReceiveCompleted();
    void SendDownloadEnd();

public:
    void Report(bool noError);
    void ReportWriteChunk(bool isError, const size_t &writeAmount, const size_t &totalSize, bool ended);

private:
    void ProcessFileMeta(std::unique_ptr<NetPacket> &&packet);
    void ProcessFileChunk(std::unique_ptr<NetPacket> &&packet);
    void ProcessFileUpload(std::unique_ptr<NetPacket> &&packet);
    void DoTask(std::unique_ptr<NetPacket> &&packet) override;

public:
    std::string TaskName() const override;

    DECLARE_SIGNAL(OnReportFileMetaInfo, std::string, std::string, size_t)
    DECLARE_SIGNAL(OnReportReceiveFileChunk, std::vector<uint8_t>, bool)
};

#endif

