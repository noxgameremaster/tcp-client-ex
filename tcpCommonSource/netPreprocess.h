
#ifndef NET_PREPROCESS_H__
#define NET_PREPROCESS_H__

#include "netservice.h"

class NetPreprocess : public NetService
{
private:
    NetPreprocess(NetObject *parent = nullptr);
    ~NetPreprocess() override;

    bool OnInitialize() override;
    void OnDeinitialize() override;

public:
    static NetPreprocess& Instance();
};

#endif

