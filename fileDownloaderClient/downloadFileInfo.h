
#ifndef DOWNLOAD_FILE_INFO_H__
#define DOWNLOAD_FILE_INFO_H__

#include "listElement.h"

class DownloadFileInfo : public ListElement
{
public:
    struct PropertyInfo
    {
        enum PropertyId
        {
            FileName,
            SavePath,
            FileSize,
            DownloadBytes,
            PropertyEnd
        };
    };
private:
    std::string m_filename;
    std::string m_savepath;
    size_t m_fileSize;
    size_t m_downloadBytes;

public:
    explicit DownloadFileInfo();
    ~DownloadFileInfo() override;

private:
    size_t ElementCount() override
    {
        return static_cast<size_t>(PropertyInfo::PropertyEnd);
    }

public:
    std::string GetElement(int index) override;
    void SetElement(int index, const std::string &value) override;

    void SetFileInfo(const std::string &filename, const std::string &savepath, const size_t size, const size_t downloadBytes);

private:
    bool Clone(const ListElement *other) override;
};

#endif

