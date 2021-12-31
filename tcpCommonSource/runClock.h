
#ifndef RUN_CLOCK_H__
#define RUN_CLOCK_H__

#include <string>

class RunClock
{
private:
    double m_initClock;
    double m_latestClock;

public:
    explicit RunClock();
    ~RunClock();
    std::string Show(bool reset = false);
    void Reset();
};

#endif

