
#ifndef FILE_STREAM_INTERFACE_H__
#define FILE_STREAM_INTERFACE_H__

#include <string>

class FileStreamInterface
{
public:
	enum FileOpenMode
	{
		NotOpen = 0x0000,
		ReadOnly = 0x0001,
		WriteOnly = 0x0002,
		ReadWrite = ReadOnly | WriteOnly,
		Append = 0x0004,
		Truncate = 0x0008,
		Text = 0x0010,
		Unbuffered = 0x0020
	};

public:
	explicit FileStreamInterface();
	virtual ~FileStreamInterface();

private:
	virtual bool isOpen() = 0;

	virtual bool Open(FileOpenMode mode, const std::string &filename = {}) = 0;
	virtual bool Close() = 0;
};

#endif



