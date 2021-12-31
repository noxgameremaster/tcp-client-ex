
#include "runclock.h"
#include "stringhelper.h"
#include <time.h>
#include <iostream>

using namespace _StringHelper;

RunClock::RunClock()
{
    Reset();

    m_initClock = m_latestClock;
}

RunClock::~RunClock()
{
    Show();
}

std::string RunClock::Show(bool reset)
{
    double cClock = static_cast<double>(clock());
    std::string str = stringFormat("end of process %s sec", std::to_string((cClock - (reset ? m_latestClock : m_initClock)) / CLOCKS_PER_SEC));

    if (reset)
        Reset();
    return str;
}

void RunClock::Reset()
{
    clock_t currentClock = clock();

    m_latestClock = currentClock;
}
