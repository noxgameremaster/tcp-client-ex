
#ifndef IO_FILE_STREAM_H__
#define IO_FILE_STREAM_H__

#include <string>
#include <fstream>
#include <memory>
#include <functional>

class IOFileStream
{
    using file_stream_type = uint8_t;
    using file_stream_pointer_type = file_stream_type *;
    using file_handler_type = std::basic_fstream<file_stream_type, std::char_traits<file_stream_type>>;
    using file_handler_pointer = std::unique_ptr<file_handler_type>;
public:
    enum class OpenMode
    {
        None,
        ReadOnly,
        WriteOnly
    };

private:
    size_t m_readPoint;
    std::string m_filename;
    file_handler_pointer m_fileHandler;

public:
    explicit IOFileStream(const std::string &fileurl = {});
    ~IOFileStream();

private:
    size_t GetCharacterCount() const;
    size_t ReadImpl(file_stream_pointer_type getStream, const size_t &length);
    size_t WriteImpl(const file_stream_pointer_type &srcStream, const size_t &length);
    bool OpenImpl(const std::string &fileurl, OpenMode mode);

public:
    bool Open(OpenMode mode, const std::string &fileurl = {});

    template <class Container>
    bool Read(Container &dest)
    {
        if (dest.empty())
            return false;

        size_t readBytes = ReadImpl(dest.data(), dest.size());

        if (readBytes == std::string::npos)
            return false;

        if (dest.size() != readBytes)
            dest.resize(readBytes);
        return true;
    }

    template <class Container>
    bool Write(const Container &src)
    {
        if (src.empty())
            return false;

        size_t writeBytes = WriteImpl(src.data(), src.size());

        return std::string::npos != writeBytes;
    }
    static bool Exist(const std::string &fileUrl);
    bool Exist() const
    {
        return Exist(m_filename);
    }
    void Close();
    static bool FileSize(const std::string &fileUrl, size_t &destSize);
    bool FileSize(size_t &destSize)
    {
        return FileSize(destSize);
    }

    std::string FileName() const
    {
        return m_filename;
    }

    static void UrlSeparatePathAndName(const std::string &url, std::string &path, std::string &name);
    void UrlSeparatePathAndName(std::string &path, std::string &name);
};

#endif

