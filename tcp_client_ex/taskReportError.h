
#ifndef TASK_REPORT_ERROR_H__
#define TASK_REPORT_ERROR_H__

#include "abstractTask.h"

class TaskReportError : public AbstractTask
{
public:
    explicit TaskReportError(NetObject *parent);
    ~TaskReportError() override;

private:
    void DoTask(std::unique_ptr<NetPacket> &&packet) override;
    std::string TaskName() const override;
};

#endif

