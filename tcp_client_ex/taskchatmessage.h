
#ifndef TASK_CHAT_MESSAGE_H__
#define TASK_CHAT_MESSAGE_H__

#include "abstracttask.h"

class TaskChatMessage : public AbstractTask
{
    using handle_pointer = void *;
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

private:
	handle_pointer m_consoleHandle;
	ConsoleColor m_oldColor;

public:
    TaskChatMessage(NetObject *parent);
    ~TaskChatMessage();

private:
	void ChangeTextColor(ConsoleColor colr);
	void GetConsoleHandlePointer();
	bool CheckValidColor(uint8_t colrbyte);
	void PrintMessage(const std::string &message, ConsoleColor colr);
	virtual void DoTask(std::unique_ptr<NetPacket> &&packet) override;

public:
	static std::string TaskName();
};

#endif

