
#include "largefile.h"
#include "eventworker.h"
#include "stringhelper.h"
#include <experimental\filesystem>
#include <fstream>

using namespace _StringHelper;

LargeFile::LargeFile()
{ }

LargeFile::~LargeFile()
{ }

bool LargeFile::MakeDirectory(const std::string &path)
{
    if (std::experimental::filesystem::is_directory(path))
        return true;

    return std::experimental::filesystem::create_directories(path);
}

bool LargeFile::RemoveIfAlreadExist(bool erase)
{
    if (m_pathname.empty())
        return false;
    if (m_filename.empty())
        return false;

    std::string fullname = stringFormat("%s\\%s", m_pathname, m_filename);

    bool isExist = std::experimental::filesystem::exists(fullname);

    if (erase && isExist)
        std::experimental::filesystem::remove(fullname);

    return isExist;
}

bool LargeFile::SetFileParams(const std::string &fileName, const std::string &pathName, const size_t &fileSize)
{
    std::unique_ptr<bool, std::function<void(bool*)>> bRet(new bool(false), [this](bool *ret)
    {
        EventWorker::Instance().AppendTask(&m_OnReportSetParamResult, *ret);
        delete ret;
    });
    if (!RemoveIfAlreadExist())
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

    if (!fileSize)
        return false;

    *bRet = true;
    m_filesize = fileSize;
    return *bRet;
}

bool LargeFile::Write(const uint8_t *stream, const size_t &length)
{
    using binary_ofstream = std::basic_ofstream<uint8_t, std::char_traits<uint8_t>>;

    binary_ofstream file(m_filename, std::ofstream::out | std::ios::app | binary_ofstream::binary);

    if (!file)
        return false;

    file.write(stream, length);

    return file.good();
}

bool LargeFile::WriteAll(const uint8_t *stream, const size_t &length)
{
    using binary_ofstream = std::basic_ofstream<uint8_t, std::char_traits<uint8_t>>;
    binary_ofstream file(m_filename, binary_ofstream::out | binary_ofstream::binary);

    if (!file)
        return false;

    file.write(stream, length);
    return file.good();
}