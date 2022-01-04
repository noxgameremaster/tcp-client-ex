
#include "taskReportError.h"
#include "reportErrorPacket.h"
#include "netLogObject.h"
#include "stringHelper.h"

using namespace _StringHelper;

TaskReportError::TaskReportError(NetObject *parent)
    : AbstractTask(parent)
{ }

TaskReportError::~TaskReportError()
{ }

void TaskReportError::DoTask(std::unique_ptr<NetPacket> &&packet)
{
    ReportErrorPacket *errReport = dynamic_cast<ReportErrorPacket *>(packet.get());

    if (nullptr == errReport)
        return;

    std::list<std::string> errorList = errReport->GetErrorInfo();

    for (const auto &s : errorList)
    {
        NetLogObject::LogObject().AppendLogMessage(stringFormat("error state: %s", s), 
            PrintUtil::ConsoleColor::COLOR_RED);
    }
}

std::string TaskReportError::TaskName() const
{
    return NetPacket::TaskKey<ReportErrorPacket>::Get();
}

