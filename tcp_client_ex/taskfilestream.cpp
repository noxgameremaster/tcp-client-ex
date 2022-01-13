
#include "taskfilestream.h"
#include "filepacket.h"
#include "filechunkpacket.h"
#include "filepacketupload.h"
#include "downloadCompletePacket.h"
#include "netLogObject.h"
#include "eventworker.h"
#include "ioFileStream.h"
#include "frameRateThread.h"
#include "runClock.h"
#include "stringhelper.h"

using namespace _StringHelper;

TaskFileStream::TaskFileStream(NetObject *parent)
    : AbstractTask(parent)
{
    m_error = 0;
    m_filesize = 0;
    m_timeCounter = std::make_unique<RunClock>();
    
    m_OnReportWritePos.Connection(&TaskFileStream::SlotWritePos, this);
}

TaskFileStream::~TaskFileStream()
{ }

void TaskFileStream::WriteFileMetaData()
{
    QUEUE_EMIT(m_OnReportFileMetaInfo, m_filename, m_pathname, m_filesize);
}

void TaskFileStream::ReportFileMetaReceiveCompleted()
{
    std::unique_ptr<FilePacket> report(new FilePacket);

    report->SetFilePacketDirection(FilePacket::FilePacketDirection::ClientToServer);
    report->SetFileName(m_filename);
    ForwardPacketToManager(std::move(report));
}

void TaskFileStream::SendDownloadEnd()
{
    std::unique_ptr<DownloadCompletePacket> endDownload(new DownloadCompletePacket);

    endDownload->SetFileSize(static_cast<uint64_t>(m_filesize));    //FIXME
    endDownload->SetFileCrc(0); //FIXME
    endDownload->ChangeSubCommand(DownloadCompletePacket::SubCmd::SendToServer);

    ForwardPacketToManager(std::move(endDownload));
}

void TaskFileStream::Report(bool noError)
{
    m_error = noError ? 0 : 1;
    //Todo. 파일 메타정보를 받았으므로 서버에 완료보고를 해야한다

    if (noError)
    {
        ReportFileMetaReceiveCompleted();
        return;
    }
    NET_PUSH_LOGMSG("taskfilestream::report error", PrintUtil::ConsoleColor::COLOR_RED);
}

void TaskFileStream::ReportWriteChunk(bool isError, const size_t &writeAmount, const size_t &totalSize, bool ended)
{
    if (ended)
    {
        m_filesize = writeAmount;
        SendDownloadEnd();
        return;
    }
    std::unique_ptr<FileChunkPacket> reportChunk(new FileChunkPacket);

    reportChunk->SetReportParam(isError, (writeAmount >= totalSize), writeAmount);
    /*reportChunk->SetSubCommand(1);*/

    reportChunk->ChangeSubCommand(FileChunkPacket::PacketSubCmd::SendToServer);

    ForwardPacketToManager(std::move(reportChunk));

    if (writeAmount >= totalSize)
    {
        std::string elapsedTime = m_timeCounter->Show(true);

        NET_PUSH_LOGMSG(stringFormat("download complete. %s seconds", elapsedTime), PrintUtil::ConsoleColor::COLOR_VIOLET);

        InnerSendFileInfo(m_filesize);
    }
    else
    {
        QUEUE_EMIT(m_OnReportWritePos, writeAmount, totalSize)
    }
}

void TaskFileStream::InnerSendFileInfo(const size_t writeAmount)
{
    std::unique_ptr<FilePacket> innerReport(new FilePacket);

    innerReport->SetFileName(m_filename);
    innerReport->SetFilesize(m_filesize);
    innerReport->SetSavePath(m_pathname);
    if (writeAmount)
        innerReport->SetDownloadBytes(writeAmount);

    ForwardPacketToManager(std::move(innerReport), true);
}

void TaskFileStream::ProcessFileMeta(std::unique_ptr<NetPacket> &&packet)
{
    FilePacket *metadata = dynamic_cast<FilePacket *>(packet.get());

    if (nullptr == metadata)
        return;

    m_filename = metadata->GetFileName();
    m_pathname = "downloads"; //metadata->GetFilePath();
    m_filesize = metadata->GetFilesize();

    WriteFileMetaData();

    NET_PUSH_LOGMSG(stringFormat("start download. %s file %d bytes...", m_filename, m_filesize), PrintUtil::ConsoleColor::COLOR_GREY);

    InnerSendFileInfo(0);

    m_timeCounter->Reset();
}

void TaskFileStream::ProcessFileChunk(std::unique_ptr<NetPacket> &&packet)
{
    FileChunkPacket *chunkdata = dynamic_cast<FileChunkPacket *>(packet.get());

    if (nullptr == chunkdata)
        return;

    bool ended = false;

    switch (chunkdata->SubCommand())
    {
    case FileChunkPacket::PacketSubCmd::SendEndChunkToClient:
        //SendDownloadEnd();
        NET_PUSH_LOGMSG("end stream");
        ended = true;
        //break;

    case FileChunkPacket::PacketSubCmd::PrevToClient:
    case FileChunkPacket::PacketSubCmd::SendToClient:
        {
            static std::vector<uint8_t> fileChunk;

            if (!chunkdata->FetchFileChunk(fileChunk))
                return;
            QUEUE_EMIT(m_OnReportReceiveFileChunk, std::move(fileChunk), ended);
            break;
        }
    }
}

void TaskFileStream::ProcessFileUpload(std::unique_ptr<NetPacket> &&packet)
{
    FilePacketUpload *uploadInfo = dynamic_cast<FilePacketUpload *>(packet.get());

    switch (uploadInfo->SubCommand())
    {
    case FilePacketUpload::PacketSubCmd::TestSendToServer:
        {
            m_uploadPath = uploadInfo->UploadPath();

            IOFileStream::UrlSeparatePathAndName(m_uploadPath, m_pathname, m_filename);
            std::string downloadPath = "downloads";

            QUEUE_EMIT(m_OnReportFileMetaInfo, m_filename, downloadPath, 0);
        }
        break;
    case FilePacketUpload::PacketSubCmd::FileServerToClient:
        {
            NET_PUSH_LOGMSG("server ready to file\n", PrintUtil::ConsoleColor::COLOR_BLUE);
            std::unique_ptr<FileChunkPacket> chunk(new FileChunkPacket);

            chunk->ChangeSubCommand(FileChunkPacket::PacketSubCmd::SendToServer);
            ForwardPacketToManager(std::move(chunk));
        }
        break;
    default:
        NET_PUSH_LOGMSG("server sent unknown packet", PrintUtil::ConsoleColor::COLOR_BLUE);
    }
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
    else if (dynamic_cast<FilePacketUpload *>(getpacket.get()) != nullptr)
        ProcessFileUpload(std::move(getpacket));
}

std::string TaskFileStream::TaskName() const
{
    return NetPacket::TaskKey<FilePacket>::Get();
}

void TaskFileStream::SlotWritePos(const size_t &writeAmount, const size_t &totalAmount)
{
    static uint32_t cFps = 0;

    if (FrameRateThread::FrameThreadObject().IsGreater(cFps, 100))
    {
        cFps = FrameRateThread::FrameThreadObject().CurrentFps();
        InnerSendFileInfo(writeAmount);
    }
}