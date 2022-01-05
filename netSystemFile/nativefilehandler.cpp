
#include "nativeFileHandler.h"
#include "fileStreamInterface.h"

#include <iostream>
#include <algorithm>

NativeFileHandler::NativeFileHandler(const std::string &filename)
	: FileEngine(filename)
{
	m_fPointer = nullptr;
	m_mappedptr = nullptr;
	m_mappedsize = 0;
}

NativeFileHandler::~NativeFileHandler()
{
	Close();
}

std::list<char> NativeFileHandler::checkFilemode(int mode)
{
	std::list<char> modelist(1, 'b');

	if (mode & FileStreamInterface::FileOpenMode::ReadOnly)
		modelist.push_front('r');
	else if (mode & FileStreamInterface::FileOpenMode::WriteOnly)
		modelist.push_front('w');
	if (mode & FileStreamInterface::FileOpenMode::Append)
		modelist.push_front('a');

	return modelist;
}

bool NativeFileHandler::openfile(int openmode)
{
	auto modelist = checkFilemode(static_cast<int>(openmode));
	std::string modestr;

	modestr.reserve(sizeof(int));

	for (const char &c : modelist)
		modestr.append(sizeof(char), c);

	fopen_s(&m_fPointer, Filename().c_str(), modestr.c_str());

	return m_fPointer != nullptr;
}

bool NativeFileHandler::closefile()
{
	if (m_fPointer == nullptr)
		return false;

	fclose(m_fPointer);
	m_fPointer = nullptr;

	return true;
}

bool NativeFileHandler::readfile(uint8_t *pData, uint32_t length)
{
	if (pData == nullptr || m_fPointer == nullptr)
		return false;

	uint32_t iSz = fread_s(pData, length, sizeof(char), length, m_fPointer);

	if (!iSz)
		return false;

	setBufferLength(iSz);
	return iSz > 0;
}

bool NativeFileHandler::writefile(const uint8_t *pData, uint32_t length)
{
	if (pData == nullptr || m_fPointer == nullptr)
		return false;

	uint32_t iSz = fwrite(pData, length, sizeof(char), m_fPointer);

	setBufferLength(iSz);

	return true;
}

