
#ifndef LARGE_FILE_H__
#define LARGE_FILE_H__

#include "ccobject.h"
#include <string>
#include <vector>

class LargeFile : public CCObject
{
private:
    std::string m_pathname;
    std::string m_filename;
    size_t m_filesize;
    size_t m_writeAmount;

public:
    explicit LargeFile();
    ~LargeFile();

private:
    bool MakeDirectory(const std::string &path);
    bool RemoveIfAlreadExist(const std::string &fullname, bool erase = true);
    std::string GetFullName();

public:
    bool SetFileParams(const std::string &fileName, const std::string &pathName, const size_t &fileSize);
    bool Write(const uint8_t *stream, const size_t &length);
    bool WriteAll(const uint8_t *stream, const size_t &length);

public:
    void SlotSetFileParams(const std::string &fileName, const std::string &pathName, const size_t &fileSize);
    void SlotWriteChunk(const std::vector<uint8_t> &srcChunk, bool ended);

    DECLARE_SIGNAL(OnReportSetParamResult, bool)
    DECLARE_SIGNAL(OnWriteChunk, bool, size_t, size_t, bool)
};

#endif

