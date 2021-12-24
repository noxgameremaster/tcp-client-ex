
#include "serverFileTask.h"
#include "filepacket.h"
#include "fileChunkPacket.h"
#include "ioFileStream.h"

#include "printUtil.h"
#include "stringHelper.h"

using namespace _StringHelper;

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

    PrintUtil::PrintMessage(PrintUtil::ConsoleColor::COLOR_WHITE, "!debug!- server sent a packet for file information");
    PrintUtil::PrintMessage(PrintUtil::ConsoleColor::COLOR_WHITE,
        stringFormat("path: %s, name: %s, size: %d bytes ...", fileinfo->GetFilePath(), fileinfo->GetFileName(), fileinfo->GetFilesize()));
}

void ServerFileTask::ServerFileStream(std::unique_ptr<NetPacket> &&fileStream)
{ }

void ServerFileTask::DoTask(std::unique_ptr<NetPacket> &&packet)
{
    if (dynamic_cast<FilePacket *>(packet.get()))
        ServerFileMeta(std::move(packet));
    else if (dynamic_cast<FileChunkPacket *>(packet.get()))
        ServerFileStream(std::move(packet));
}

std::string ServerFileTask::TaskName() const
{
    return NetPacket::TaskKey<FilePacket>::Get();
}

