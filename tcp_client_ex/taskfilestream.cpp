
#include "taskfilestream.h"
#include "filepacket.h"
#include "filechunkpacket.h"
#include "printutil.h"
#include "eventworker.h"
#include "stringhelper.h"

using namespace _StringHelper;

TaskFileStream::TaskFileStream(NetObject *parent)
    : AbstractTask(parent)
{
    m_error = 0;
    m_filesize = 0;
}

TaskFileStream::~TaskFileStream()
{ }

void TaskFileStream::WriteFileMetaData()
{
    EventWorker &work = EventWorker::Instance();

    work.AppendTask(&m_OnReportFileMetaInfo, m_filename, m_pathname, m_filesize);
}

void TaskFileStream::ReportFileMetaReceiveCompleted()
{
    std::unique_ptr<FilePacket> report(new FilePacket);

    report->SetError(m_error);
    report->SetFileName(m_filename);

    ForwardPacketToManager(std::move(report));
}

void TaskFileStream::Report(bool noError)
{
    m_error = noError ? 0 : 1;
    //Todo. 파일 메타정보를 받았으므로 서버에 완료보고를 해야한다
    ReportFileMetaReceiveCompleted();
}

void TaskFileStream::ReportWriteChunk(bool isError, const size_t &writeAmount, const size_t &totalSize)
{
    std::unique_ptr<FileChunkPacket> reportChunk(new FileChunkPacket);

    reportChunk->SetReportParam(isError, (writeAmount >= totalSize), writeAmount);
    ForwardPacketToManager(std::move(reportChunk));
    PrintUtil::PrintMessage(PrintUtil::ConsoleColor::COLOR_CYAN, stringFormat("report progress (%d of %d)...", writeAmount, totalSize));
}

void TaskFileStream::ProcessFileMeta(std::unique_ptr<NetPacket> &&packet)
{
    FilePacket *metadata = dynamic_cast<FilePacket *>(packet.get());

    if (nullptr == metadata)
        return;

    m_filename = metadata->GetFileName();
    m_pathname = metadata->GetFilePath();
    m_filesize = metadata->GetFilesize();

    WriteFileMetaData();
}

void TaskFileStream::ProcessFileChunk(std::unique_ptr<NetPacket> &&packet)
{
    FileChunkPacket *chunkdata = dynamic_cast<FileChunkPacket *>(packet.get());

    if (nullptr == chunkdata)
        return;

    std::vector<uint8_t> fileChunk;

    if (!chunkdata->FetchFileChunk(fileChunk))
        return;

    EventWorker::Instance().AppendTask(&m_OnReportReceiveFileChunk, std::move(fileChunk));
}

void TaskFileStream::DoTask(std::unique_ptr<NetPacket> &&packet)
{
    if (!packet)
        return;

    std::remove_reference<decltype(packet)>::type getpacket = std::move(packet);

    if (dynamic_cast<FilePacket *>(getpacket.get()) != nullptr)
        ProcessFileMeta(std::move(getpacket));
    else if (dynamic_cast<FileChunkPacket *>(getpacket.get()) != nullptr)
        ProcessFileChunk(std::move(getpacket));
}

std::string TaskFileStream::TaskName() const
{
    return NetPacket::TaskKey<FilePacket>::Get();
}
