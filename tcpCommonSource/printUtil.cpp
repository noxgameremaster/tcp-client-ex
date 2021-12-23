
#include "printutil.h"
#include <windows.h>

std::mutex PrintUtil::s_lock;

void PrintUtil::ChangeTextColor(ConsoleColor colr)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(colr));
}
