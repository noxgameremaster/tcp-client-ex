
#include "netPreprocess.h"
#include "frameRateThread.h"
#include "netLogObject.h"
#include "eventworker.h"

NetPreprocess::NetPreprocess(NetObject *parent)
    : NetService(parent)
{ }

NetPreprocess::~NetPreprocess()
{ }

bool NetPreprocess::OnInitialize()
{
    NetLogObject::LogObject().Startup();
    FrameRateThread::FrameThreadObject().Startup();
    EventWorker::Instance();

    return true;
}

void NetPreprocess::OnDeinitialize()
{
    FrameRateThread::FrameThreadObject().Shutdown();
    NetLogObject::LogObject().Shutdown();
}

NetPreprocess& NetPreprocess::Instance()
{
    static NetPreprocess pre;

    return pre;
}

