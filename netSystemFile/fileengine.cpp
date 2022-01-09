

#include "fileEngine.h"
#include "fileStreamInterface.h"
//#include "utils/printing.h"
#include <windows.h>

#define MAKE64INT(X, Y)			(DWORD64)((DWORD64)X | ((DWORD64)Y << 32))


FileEngine::FileEngine(const std::string &filename)
	: m_maxBlocksize(100)
{
	m_filename = filename;
	m_openMode = FileStreamInterface::FileOpenMode::NotOpen;
	m_hMap = nullptr;
	m_hFile = nullptr;
	m_iLastBuffLength = 0;
}

FileEngine::~FileEngine()
{
	reset();
}

bool FileEngine::Opened() const
{
	return m_openMode != FileStreamInterface::FileOpenMode::NotOpen;
}

bool FileEngine::ReadWithVector(std::vector<uint8_t> &readvec)
{
	if (!readvec.size())
		return false;

	uint32_t iReadc = 0;
	bool pResult = Read(readvec.data(), readvec.size(), &iReadc);

	if (readvec.size() != iReadc)
		readvec.resize(iReadc);

	return pResult;
}

bool FileEngine::WriteWithVector(const std::vector<uint8_t> &srcvec)
{
	if (!srcvec.size())
		return false;

	return Write(srcvec.data(), srcvec.size(), nullptr);
}

bool FileEngine::deinitMaphandle()
{
	if (m_hMap == nullptr)
		return false;

	::CloseHandle(m_hMap);
	m_hMap = nullptr;

	return true;
}

bool FileEngine::create(uint32_t accessmode, uint32_t dwCreateFlag)
{
	if (!m_filename.length())
		return false;

	reset();
	m_hFile = ::CreateFileA(m_filename.c_str(), accessmode, 0, nullptr, dwCreateFlag, FILE_ATTRIBUTE_NORMAL, nullptr);

	bool pResult = m_hFile != INVALID_HANDLE_VALUE;

	if (!pResult)
		m_hFile = nullptr;

	return pResult;
}

bool FileEngine::reset()
{
	if (m_hFile == nullptr)
		return false;

	::CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;
	return true;
}

bool FileEngine::mapp(uint64_t offset, uint64_t size, uint8_t *&destmapAddr)
{
	if (!create(GENERIC_READ | GENERIC_WRITE, OPEN_EXISTING))
		return false;

	DWORD dwProtect = PAGE_READWRITE;
	m_hMap = ::CreateFileMapping(static_cast<HANDLE>(m_hFile), nullptr, dwProtect, size >> 32, static_cast<DWORD>(size), nullptr);

	if (m_hMap == INVALID_HANDLE_VALUE)
	{
		deinitMaphandle();
		return false;
	}

	DWORD dwAccess = FILE_MAP_READ | FILE_MAP_WRITE;

	DWORD offsetHi = offset >> 32;
	DWORD offsetLo = offset & 0xffffffff;
	SYSTEM_INFO sysinfo;
	::GetSystemInfo(&sysinfo);
	DWORD mask = sysinfo.dwAllocationGranularity - 1;
	DWORD extra = offset & mask;
	if (extra)
		offsetLo &= ~mask;

	// attempt to create the map
	LPVOID mapAddress = ::MapViewOfFile(m_hMap, dwAccess, offsetHi, offsetLo, static_cast<DWORD>(size) + extra);

	if (mapAddress)
	{
		uint8_t *address = extra + static_cast<uint8_t *>(mapAddress);
		m_maps[address] = extra;

		destmapAddr = address;

		return true;
	}
	deinitMaphandle();
	return false;
}

bool FileEngine::unmapp(uint8_t *ptr)
{
	auto keyIterator = m_maps.find(ptr);

	if (keyIterator == m_maps.end())
		return false;

	uint8_t *start = ptr - keyIterator->second;
	if (!::UnmapViewOfFile(start))
		return false;

	m_maps.erase(keyIterator);
	if (m_maps.empty())
		deinitMaphandle();
	return true;
}

bool FileEngine::Open(int openmode)
{
	if (Opened() || openmode == FileStreamInterface::FileOpenMode::NotOpen)
		return false;

	if (!openfile(openmode))
		return false;

	m_openMode = openmode;
	return true;
}

bool FileEngine::Close()
{
	if (m_openMode == FileStreamInterface::FileOpenMode::NotOpen)
		return false;

	closefile();
	m_openMode = FileStreamInterface::FileOpenMode::NotOpen;
	return true;
}

bool FileEngine::Read(uint8_t *pData, uint32_t length, uint32_t *iReadc)
{
	bool pResult = readfile(pData, length);

	if (iReadc != nullptr)
		*iReadc = m_iLastBuffLength;
	return pResult;
}

bool FileEngine::Write(const uint8_t *pData, uint32_t length, uint32_t *iWrtc)
{
	bool pResult = writefile(pData, length);

	if (iWrtc != nullptr)
		*iWrtc = m_iLastBuffLength;
	return pResult;
}

bool FileEngine::Mapping(uint64_t offset, uint64_t size, uint8_t *&destptr)
{
	if (!mapp(offset, size, destptr))
	{
		//_printUtil::Print("mapping error::", std::to_string(::GetLastError()));
		return false;
	}
	return true;
}

bool FileEngine::Unmapping(uint8_t *ptr)
{
	if (ptr == nullptr)
		return false;

	return unmapp(ptr);
}

bool FileEngine::GetMapExtra(uint8_t *pAddr, uint32_t &iExtra)
{
	auto keyIterator = m_maps.find(pAddr);

	if (keyIterator == m_maps.end())
		return false;

	iExtra = keyIterator->second;
	return true;
}

bool FileEngine::Filesize(uint64_t &getsize)
{
    WIN32_FILE_ATTRIBUTE_DATA attrData = { };

	if (!GetFileAttributesExA(m_filename.c_str(), GetFileExInfoStandard, &attrData))
		return false;

	LARGE_INTEGER size;

	size.HighPart = attrData.nFileSizeHigh;
	size.LowPart = attrData.nFileSizeLow;
	getsize = size.QuadPart;

	return true;
}

bool FileEngine::FileCopy(const std::string &copiedFilename)
{
	return ::CopyFileA(m_filename.c_str(), copiedFilename.c_str(), true) ? true : false;
}

void FileEngine::setBufferLength(uint32_t length)
{
	m_iLastBuffLength = length;
}

