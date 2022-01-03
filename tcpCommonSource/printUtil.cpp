
#include "printutil.h"
#include <windows.h>

std::mutex PrintUtil::s_lock;

void PrintUtil::ChangeTextColor(ConsoleColor colr)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(colr));
}

PrintUtil::ConsoleColor PrintUtil::GetPrintColor(uint32_t id)
{
    auto EnumToInt = [](ConsoleColor colr) { return static_cast<uint32_t>(colr); };

    if (id > EnumToInt(ConsoleColor::COLOR_MIN) && id < EnumToInt(ConsoleColor::COLOR_MAX))
        return static_cast<ConsoleColor>(id);

    return ConsoleColor::COLOR_WHITE;
}
