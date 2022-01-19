
#ifndef COMPLETED_FILE_INFO_H__
#define COMPLETED_FILE_INFO_H__

#include "listElement.h"

class CompletedFileInfo : public ListElement
{
public:
    struct PropertyInfo
    {
        enum PropertyId
        {
            FileUrl,
            FileSize,
            DownloadDateTime,
            PropertyEnd
        };
    };
private:
    std::string m_fileUrl;
    size_t m_fileSize;
    std::string m_downloadDateTime;
    const uint32_t m_completeSequence;

public:
    explicit CompletedFileInfo();
    ~CompletedFileInfo() override;

private:
    size_t ElementCount() const override
    {
        return static_cast<size_t>(PropertyInfo::PropertyEnd);
    }

    virtual std::string GetUniqId() const
    {
        return std::to_string(m_completeSequence);
    }

public:
    std::string GetElement(int index) override;
    void SetElement(int index, const std::string &value) override;
    void SetCreatedDateTime();
};

#endif

