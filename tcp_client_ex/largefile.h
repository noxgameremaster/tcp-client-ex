
#ifndef LARGE_FILE_H__
#define LARGE_FILE_H__

#include "ccobject.h"
#include <string>

class LargeFile : public CCObject
{
private:
    std::string m_pathname;
    std::string m_filename;
    size_t m_filesize;

public:
    explicit LargeFile();
    ~LargeFile();

private:
    bool MakeDirectory(const std::string &path);
    bool RemoveIfAlreadExist(bool erase = true);

public:
    bool SetFileParams(const std::string &fileName, const std::string &pathName, const size_t &fileSize);
    bool Write(const uint8_t *stream, const size_t &length);
    bool WriteAll(const uint8_t *stream, const size_t &length);

    DECLARE_SIGNAL(OnReportSetParamResult, bool)
};

#endif

