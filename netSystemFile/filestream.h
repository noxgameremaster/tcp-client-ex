
#ifndef FILE_STREAM_H__
#define FILE_STREAM_H__

#include "fileStreamInterface.h"
#include <memory>
#include <vector>

class FileEngine;

/**
* @class. File I/O for large files
* @brief. a class which implements many native methods of the WinAPI.
*/
class FileStream : public FileStreamInterface
{
private:
	std::string m_filename;
	std::unique_ptr<FileEngine> m_fileHandler;

public:
	explicit FileStream(const std::string &filename = {});
	virtual ~FileStream() override;

public:
	virtual bool isOpen() override;

	virtual bool Open(FileOpenMode mode, const std::string &filename = {}) override;
	virtual bool Close() override;

	bool Read(uint8_t *pData, uint32_t length, uint32_t &iReadsize);
	bool Read(uint8_t *pData, uint32_t length);
	bool ReadVector(std::vector<uint8_t> &dest);

	bool Write(const uint8_t *pData, uint32_t length, uint32_t &iWritesize);
	bool Write(const uint8_t *pData, uint32_t length);
	bool WriteVector(const std::vector<uint8_t> &srcvect);

	/**
* File Mapping
*
* @param offset. an offset
* @param size. file size
* @param pointer destptr. destination pointer
*
* @return success or failure
*/
	bool Map(uint64_t offset, uint64_t size, uint8_t *&destptr);
	bool Unmap(uint8_t *ptr);

	std::string Filename() const {
		return m_filename;
	}
	uint64_t Filesize() const;

	std::string OnlyFilename() const;
	static std::string OnlyFilename(const std::string &filename);
	std::string GetFilePath();
	static std::string GetFilePath(const std::string &filename);
	bool Exist();
	static bool Exist(const std::string &filename);

	bool Copy(const std::string &copiedname);

};

#endif

