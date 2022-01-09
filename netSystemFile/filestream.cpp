

#include "fileStream.h"
#include "fileEngine.h"
#include "nativeFileHandler.h"
#include <algorithm>
#include <filesystem>

#define VISUAL_STUDIO_2015 1900
#if _MSC_VER == VISUAL_STUDIO_2015
#define NAMESPACE_FILESYSTEM std::experimental::filesystem
#else
#define NAMESPACE_FILESYSTEM std::filesystem
#endif
#undef VISUAL_STUDIO_2015


using fileHandlerType = NativeFileHandler;

FileStream::FileStream(const std::string &filename)
	: FileStreamInterface()
{
	m_filename = filename;
	m_fileHandler = std::make_unique<fileHandlerType>();
}

FileStream::~FileStream()
{
	m_fileHandler.reset();
}

bool FileStream::isOpen()
{
	if (!m_fileHandler)
		return false;

	return m_fileHandler->Opened();
}

bool FileStream::Open(FileStreamInterface::FileOpenMode mode, const std::string &filename)
{
	if (filename.length())
		m_filename = filename;
	m_fileHandler = std::make_unique<fileHandlerType>(m_filename);

	return m_fileHandler->Open(mode);
}

bool FileStream::Close()
{
	if (m_fileHandler == nullptr)
		return false;

	return m_fileHandler->Close();
}

bool FileStream::Read(uint8_t *pData, uint32_t length, uint32_t &iReadsize)
{
	if (!isOpen())
		return false;

	return m_fileHandler->Read(pData, length, &iReadsize);
}

bool FileStream::Read(uint8_t *pData, uint32_t length)
{
	uint32_t iDiscard;

	return Read(pData, length, iDiscard);
}

bool FileStream::ReadVector(std::vector<uint8_t> &dest)
{
	if (!isOpen())
		return false;

	return m_fileHandler->ReadWithVector(dest);
}

bool FileStream::Write(const uint8_t *pData, uint32_t length, uint32_t &iWritesize)
{
	if (!isOpen())
		return false;

	return m_fileHandler->Write(pData, length, &iWritesize);
}

bool FileStream::Write(const uint8_t *pData, uint32_t length)
{
	uint32_t iDiscard;

	return Write(pData, length, iDiscard);
}

bool FileStream::WriteVector(const std::vector<uint8_t> &srcvect)
{
	if (!isOpen())
		return false;

	return m_fileHandler->WriteWithVector(srcvect);
}

bool FileStream::Map(uint64_t offset, uint64_t size, uint8_t *&destptr)
{
	if (!m_fileHandler)
		return false;

	if (isOpen())
		Close();
	return m_fileHandler->Mapping(offset, size, destptr);
}

bool FileStream::Unmap(uint8_t *ptr)
{
	if (!m_fileHandler)
		return false;

	return m_fileHandler->Unmapping(ptr);
}

uint64_t FileStream::Filesize() const
{
	uint64_t getsize = 0;

	if (!m_fileHandler->Filesize(getsize))
		return 0;

	return getsize;
}

std::string FileStream::OnlyFilename() const
{
	return OnlyFilename(m_filename);
}

std::string FileStream::OnlyFilename(const std::string &filename)
{
	uint32_t iMedSep = filename.find_last_of('\\');

	return filename.substr(iMedSep ? iMedSep + 1 : 0);
}

std::string FileStream::GetFilePath()
{
	return GetFilePath(m_filename);
}

std::string FileStream::GetFilePath(const std::string &filename)
{
	uint32_t iMedSep = filename.find_last_of('\\');

	return filename.substr(0, iMedSep);
}

bool FileStream::Exist()
{
	return Exist(m_filename);
}

bool FileStream::Exist(const std::string &filename)
{
	return NAMESPACE_FILESYSTEM::exists(filename);
}

bool FileStream::Copy(const std::string &copiedname)
{
	return m_fileHandler->FileCopy(copiedname);
}


