
#ifndef TASK_CHAT_MESSAGE_H__
#define TASK_CHAT_MESSAGE_H__

#include "abstracttask.h"

class TaskChatMessage : public AbstractTask
{
public:
    TaskChatMessage(NetObject *parent);
    ~TaskChatMessage();

private:
	bool CheckValidColor(uint8_t colrbyte);
	void PrintMessage(const std::string &message, uint8_t colr);
	virtual void DoTask(std::unique_ptr<NetPacket> &&packet) override;

public:
	static std::string TaskName();

};

#endif

