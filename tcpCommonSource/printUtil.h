
#ifndef PRINT_UTIL_H__
#define PRINT_UTIL_H__

#include <iostream>
#include <mutex>

class PrintUtil
{
public:
    enum class ConsoleColor
    {
        COLOR_MIN,
        COLOR_DARKBLUE = 1,
        COLOR_GREEN,
        COLOR_EMERALD,
        COLOR_DARKRED,
        COLOR_VIOLET,
        COLOR_DARKYELLOW,
        COLOR_DARKWHITE = 7,
        COLOR_GREY = 8,
        COLOR_BLUE = 9,
        COLOR_LIGHTGREEN = 10,
        COLOR_CYAN,
        COLOR_RED,
        COLOR_PINK,
        COLOR_YELLOW,
        COLOR_WHITE,
        COLOR_MAX
    };

public:
    PrintUtil() = delete;
    ~PrintUtil() = delete;

private:
    template <class Arg>
    static void Print(Arg &&arg)
    {
        std::cout << std::forward<Arg>(arg) << std::endl;
    }

    template <class Arg, class... Args>
    static void Print(Arg &&arg, Args&&... args)
    {
        std::cout << std::forward<Arg>(arg) << ' ';
        Print(std::forward<Args>(args)...);
    }

private:
    static void ChangeTextColor(ConsoleColor colr);

public:
    template <class... Args>
    static void PrintMessage(Args&&... args)
    {
        {
            std::lock_guard<std::mutex> guard(s_lock);

            Print(std::forward<Args>(args)...);
        }
    }

    template <class... Args>
    static void PrintMessage(ConsoleColor colr, Args&&... args)
    {
        {
            std::lock_guard<std::mutex> guard(s_lock);

            ChangeTextColor(colr);
            Print(std::forward<Args>(args)...);
            ChangeTextColor(ConsoleColor::COLOR_DARKWHITE);
        }
    }

    static ConsoleColor GetPrintColor(uint32_t id);

private:
    static std::mutex s_lock;
};

#endif

