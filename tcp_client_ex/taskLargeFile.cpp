
#include "taskLargeFile.h"
#include "largeFileRequestPacket.h"
#include "largeFileChunkPacket.h"
#include "largeFileCompletePacket.h"
#include "filepacket.h"
#include "netLogObject.h"
#include "asynchronousFileTask.h"
#include "ioFileStream.h"
#include "eventworker.h"
#include "frameRateThread.h"
#include "stringHelper.h"

using namespace _StringHelper;

TaskLargeFile::TaskLargeFile(NetObject *parent)
    : AbstractTask(parent)
{
    m_accumulate = 0;
    m_OnReportFileSeek.Connection(&TaskLargeFile::SlotReportFileSeek, this);
}

TaskLargeFile::~TaskLargeFile()
{ }

void TaskLargeFile::InnerSendFileInfo(const size_t amount, bool isLast)
{
    if (!m_file)
        return;

    std::string path, name;

    (*m_file)->UrlSeparatePathAndName(path, name);
    
    std::unique_ptr<FilePacket> innerSend(new FilePacket);

    innerSend->SetFileName(name);
    innerSend->SetSavePath(path);
    innerSend->SetDownloadBytes(amount);
    if (isLast)
        innerSend->SetComplete();
    ForwardPacketToManager(std::move(innerSend), true);
}

bool TaskLargeFile::CreateDownloadFile(const std::string &url)
{
    std::string filename, path;
    IOFileStream::UrlSeparatePathAndName(url, path, filename);

    m_file = std::make_unique<AsynchonousFileTask>(new IOFileStream(stringFormat("%s\\%s", "downloads", filename)));

    return (*m_file)->Open(IOFileStream::OpenMode::WriteOnly);
}

void TaskLargeFile::RequestChunkData()
{
    std::unique_ptr<LargeFileChunkPacket> reqChunk(new LargeFileChunkPacket);

    reqChunk->ChangeSubCommand(LargeFileChunkPacket::PacketSubCmd::SendToServer);
    ForwardPacketToManager(std::move(reqChunk));
}

void TaskLargeFile::ReportDownloadComplete()
{
    std::unique_ptr<LargeFileCompletePacket> complete(new LargeFileCompletePacket);

    complete->SetSubCmd();
    complete->SetLargeFileSize(static_cast<uint64_t>(m_accumulate), 0);
    ForwardPacketToManager(std::move(complete));
}

void TaskLargeFile::LargeFileRequest(std::unique_ptr<NetPacket> &&reqPacket)
{
    LargeFileRequestPacket *req = dynamic_cast<LargeFileRequestPacket *>(reqPacket.get());

    switch (req->SubCommand())
    {
    case LargeFileRequestPacket::PacketSubCmd::StartTestToServer:        
        if (!CreateDownloadFile(req->RequestFileUrl()))
        {
            NetLogObject::LogObject().AppendLogMessage("open fail!");
            break;
        }
        NET_PUSH_LOGMSG("sent request packet to server!");
        req->ChangeSubCommand(LargeFileRequestPacket::PacketSubCmd::SendToServer);
        ForwardPacketToManager(std::move(reqPacket));
        InnerSendFileInfo(0);
        break;
    case LargeFileRequestPacket::PacketSubCmd::SendToClient:
        NET_PUSH_LOGMSG("receive msg from server. i will request a chunk data!");
        RequestChunkData();
        m_accumulate = 0;
        break;
    }
}

void TaskLargeFile::LargeFileGetChunk(std::unique_ptr<NetPacket> &&pack)
{
    LargeFileChunkPacket *chunk = static_cast<LargeFileChunkPacket *>(pack.get());
    std::vector<uint8_t> stream;
    bool notCompleted = false;

    switch (chunk->SubCommand())
    {
    case LargeFileChunkPacket::PacketSubCmd::SendToClient:
        RequestChunkData();
        notCompleted = true;
        [[fallthrough]];
    case LargeFileChunkPacket::PacketSubCmd::SentLastDataToClient:
        chunk->GetFileStream(stream);
        m_file->PushStream(stream);

        m_accumulate += stream.size();
        if (notCompleted)
        {
            QUEUE_EMIT(m_OnReportFileSeek, m_accumulate);
        }
        else
        {
            ReportDownloadComplete();
            NET_PUSH_LOGMSG(stringFormat("end all %d bytes", m_accumulate));
            InnerSendFileInfo(m_accumulate, true);
        }
        break;

    default:
        break;
    }
}

void TaskLargeFile::DoTask(std::unique_ptr<NetPacket> &&packet)
{
    if (dynamic_cast<LargeFileChunkPacket *>(packet.get()) != nullptr)
        LargeFileGetChunk(std::move(packet));
    else if (dynamic_cast<LargeFileRequestPacket *>(packet.get()) != nullptr)
        LargeFileRequest(std::move(packet));
}

std::string TaskLargeFile::TaskName() const
{
    return NetPacket::TaskKey<LargeFileRequestPacket>::Get();
}

void TaskLargeFile::SlotReportFileSeek(const size_t writeAmount)
{
    static uint32_t cFps = 0;

    if (FrameRateThread::FrameThreadObject().IsGreater(cFps, 100))
    {
        cFps = FrameRateThread::FrameThreadObject().CurrentFps();
        InnerSendFileInfo(writeAmount);
    }
}