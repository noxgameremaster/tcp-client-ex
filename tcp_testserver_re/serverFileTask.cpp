
#include "serverFileTask.h"
#include "filepacket.h"
#include "fileChunkPacket.h"
#include "filepacketupload.h"

#include "ioFileStream.h"

#include "printUtil.h"

ServerFileTask::ServerFileTask(NetObject *parent)
    : ServerTask(parent)
{ }

ServerFileTask::~ServerFileTask()
{ }

void ServerFileTask::ServerFileMeta(std::unique_ptr<NetPacket> &&fileMeta)
{
    FilePacket *fileinfo = dynamic_cast<FilePacket *>(fileMeta.get());

    if (nullptr == fileinfo)
        return;

    m_filename = fileinfo->GetFileName();
    m_OnReceiveFileInfo.Emit(m_filename);
}

void ServerFileTask::ServerFileStream(std::unique_ptr<NetPacket> &&fileStream)
{
    FileChunkPacket *chunkInfo = dynamic_cast<FileChunkPacket *>(fileStream.get());

    if (chunkInfo == nullptr)
        return;

    bool err = false, end = false;
    size_t writePos = 0;

    chunkInfo->GetProgressStatus(err, end, writePos);

    if (err)
    {
        PrintUtil::PrintMessage(PrintUtil::ConsoleColor::COLOR_RED, "something wrong!");
        return;
    }
    if (end)
    {
        PrintUtil::PrintMessage(PrintUtil::ConsoleColor::COLOR_CYAN, "end of process");
        return;
    }

    //PrintUtil::PrintMessage(PrintUtil::ConsoleColor::COLOR_WHITE, "serverfiletask::serverfilestream");
    m_OnReceiveFileInfo.Emit(m_filename);
}

void ServerFileTask::ClientFileRequest(std::unique_ptr<NetPacket> &&req)
{
    FilePacketUpload *up = dynamic_cast<FilePacketUpload *>(req.get());
    //Todo. 파일 없을 시 없다고 내려
    if (!IOFileStream::Exist(up->UploadPath()))
        return;


}

void ServerFileTask::DoTask(std::unique_ptr<NetPacket> &&packet)
{
    if (dynamic_cast<FilePacket *>(packet.get()))
        ServerFileMeta(std::move(packet));
    else if (dynamic_cast<FileChunkPacket *>(packet.get()))
        ServerFileStream(std::move(packet));
    else if (dynamic_cast<FilePacketUpload *>(packet.get()))
        ClientFileRequest(std::move(packet));
}

std::string ServerFileTask::TaskName() const
{
    return NetPacket::TaskKey<FilePacket>::Get();
}

void ServerFileTask::SendFileStream(const std::vector<uint8_t> &stream, const std::string &filename)
{
    //PrintUtil::PrintMessage(PrintUtil::ConsoleColor::COLOR_WHITE, "server sent file stream");

    std::unique_ptr<FileChunkPacket> chunk(new FileChunkPacket);

    chunk->SetFileChunk(stream);
    chunk->SetFileUrl(filename);
    chunk->ChangeSubCommand(FileChunkPacket::PacketSubCmd::PrevToClient);

    ForwardPacket(std::move(chunk));
}

