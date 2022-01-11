
#ifndef DOWNLOAD_FILE_INFO_H__
#define DOWNLOAD_FILE_INFO_H__

#include "listElement.h"

class DownloadFileInfo : public ListElement
{
private:
    std::string m_filename;
    size_t m_fileSize;
    size_t m_downloadBytes;

public:
    explicit DownloadFileInfo();
    ~DownloadFileInfo() override;

private:
    size_t ElementCount() override
    {
        return 3;
    }

public:
    std::string GetElement(int index) override;
    void SetElement(int index, const std::string &value) override;

};

#endif

