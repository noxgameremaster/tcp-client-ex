
#include "largefile.h"
#include "eventworker.h"
#include "stringhelper.h"

#include <filesystem>

#define VISUAL_STUDIO_2015 1900
#if _MSC_VER == VISUAL_STUDIO_2015
#define NAMESPACE_FILESYSTEM std::experimental::filesystem
#else
#define NAMESPACE_FILESYSTEM std::filesystem
#endif
#undef VISUAL_STUDIO_2015

#include <fstream>

using namespace _StringHelper;

LargeFile::LargeFile()
{
    m_filesize = 0;
    m_writeAmount = 0;
}

LargeFile::~LargeFile()
{ }

bool LargeFile::MakeDirectory(const std::string &path)
{
    if (NAMESPACE_FILESYSTEM::is_directory(path))
        return true;

    return NAMESPACE_FILESYSTEM::create_directories(path);
}

bool LargeFile::RemoveIfAlreadExist(const std::string &fullname, bool erase)
{
    if (fullname.empty())
        return false;

    bool isExist = NAMESPACE_FILESYSTEM::exists(fullname);

    if (erase && isExist)
        NAMESPACE_FILESYSTEM::remove(fullname);

    return isExist;
}

std::string LargeFile::GetFullName()
{
    if (m_filename.empty() || m_pathname.empty())
        return {};

    return stringFormat("%s\\%s", m_pathname, m_filename);
}

bool LargeFile::SetFileParams(const std::string &fileName, const std::string &pathName, const size_t &fileSize)
{
    std::unique_ptr<bool, std::function<void(bool*)>> bRet(new bool(false), [this](bool *ret)
    {
        QUEUE_EMIT(m_OnReportSetParamResult, *ret);
        delete ret;
    });
    if (!RemoveIfAlreadExist(stringFormat("%s\\%s", pathName, fileName)))
    {
        if (!MakeDirectory(pathName))
            return false;
    }

    auto pathValidChecker = [](const std::string &src, std::string &dest)
    {
        if (src.empty())
            throw false;

        dest = src;
    };

    try
    {
        pathValidChecker(fileName, m_filename);
        pathValidChecker(pathName, m_pathname);
    }
    catch (const bool &fail)
    {
        return fail;
    }

    /*if (!fileSize)
        return false;*/

    *bRet = true;
    m_filesize = fileSize;
    m_writeAmount = 0;
    return *bRet;
}

bool LargeFile::Write(const uint8_t *stream, const size_t &length)
{
    using binary_ofstream = std::basic_ofstream<uint8_t, std::char_traits<uint8_t>>;

    binary_ofstream file(GetFullName(), std::ofstream::out | std::ios::app | binary_ofstream::binary);

    //Overlapped todo
    if (!file)
        return false;

    file.write(stream, length);

    return file.good();
}

bool LargeFile::WriteAll(const uint8_t *stream, const size_t &length)
{
    using binary_ofstream = std::basic_ofstream<uint8_t, std::char_traits<uint8_t>>;
    binary_ofstream file(GetFullName(), binary_ofstream::out | binary_ofstream::binary);

    if (!file)
        return false;

    file.write(stream, length);
    return file.good();
}

void LargeFile::SlotSetFileParams(const std::string &fileName, const std::string &pathName, const size_t &fileSize)
{
    SetFileParams(fileName, pathName, fileSize);
}

void LargeFile::SlotWriteChunk(const std::vector<uint8_t> &srcChunk, bool ended)
{
    bool result = Write(srcChunk.data(), srcChunk.size());

    if (result)
        m_writeAmount += srcChunk.size();
    QUEUE_EMIT(m_OnWriteChunk, result ? false : true, m_writeAmount, m_filesize, ended);
}

