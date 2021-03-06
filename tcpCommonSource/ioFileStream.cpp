
#include "ioFileStream.h"

#include <filesystem>

#define VISUAL_STUDIO_2015 1900
#if _MSC_VER == VISUAL_STUDIO_2015
#define NAMESPACE_FILESYSTEM std::experimental::filesystem
#else
#define NAMESPACE_FILESYSTEM std::filesystem
#endif
#undef VISUAL_STUDIO_2015

IOFileStream::IOFileStream(const std::string &fileurl)
{
    m_readPoint = 0;
    if (fileurl.length())
        m_filename = fileurl;
}

IOFileStream::~IOFileStream()
{ }

size_t IOFileStream::GetCharacterCount() const
{
    return m_fileHandler ? static_cast<size_t>(m_fileHandler->gcount()) : std::string::npos;
}

size_t IOFileStream::ReadImpl(file_stream_pointer_type getStream, const size_t &length)
{
    if (!m_fileHandler)
        return std::string::npos;
    
    m_fileHandler->read(getStream, length);
    return GetCharacterCount();
}

size_t IOFileStream::WriteImpl(const uint8_t *srcStream, const size_t &length)
{
    if (!m_fileHandler)
        return std::string::npos;

    m_fileHandler->write(srcStream, length);
    return GetCharacterCount();
}

bool IOFileStream::OpenImpl(const std::string &fileurl, OpenMode mode)
{
    uint8_t openmode = std::ios::binary;

    if (mode == OpenMode::ReadOnly)
        openmode |= std::ios::in;
    else if (mode == OpenMode::WriteOnly)
        openmode |= std::ios::out;

    m_fileHandler = std::make_unique<file_handler_type>(fileurl, openmode);
    *m_fileHandler << std::noskipws;
    return m_fileHandler->is_open();
}

bool IOFileStream::Open(OpenMode mode, const std::string &fileurl)
{
    if (fileurl.length())
    {
        if (m_filename != fileurl)
            m_filename = fileurl;
    }
    return OpenImpl(m_filename, mode);
}

bool IOFileStream::Exist(const std::string &fileUrl)
{
    return NAMESPACE_FILESYSTEM::exists(fileUrl);
}

void IOFileStream::Close()
{
    if (m_fileHandler)
        m_fileHandler.reset();
}

bool IOFileStream::FileSize(const std::string &fileUrl, uint64_t &destSize)
{
    if (!Exist(fileUrl))
        return false;

    destSize = NAMESPACE_FILESYSTEM::file_size(fileUrl);
    return true;
}

void IOFileStream::UrlSeparatePathAndName(const std::string &url, std::string &path, std::string &name)
{
    size_t findpos = url.find_last_of('\\');
    bool hasDirToken = std::string::npos == findpos;

    path = hasDirToken ? "" : url.substr(0, findpos);
    name = hasDirToken ? url : url.substr(findpos+1);
}

void IOFileStream::UrlSeparatePathAndName(std::string &path, std::string &name)
{
    UrlSeparatePathAndName(m_filename, path, name);
}