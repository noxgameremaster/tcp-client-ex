
#include "pch.h"
#include "completedFileInfo.h"

#include <iomanip>
#include <ctime>

static uint32_t s_complete_sequence = 0;

CompletedFileInfo::CompletedFileInfo()
    : ListElement(), m_completeSequence(s_complete_sequence++)
{
    m_fileSize = 0;
}

CompletedFileInfo::~CompletedFileInfo()
{ }

std::string CompletedFileInfo::GetElement(int index)
{
    switch (index)
    {
    case PropertyInfo::DownloadDateTime: return m_downloadDateTime;
    case PropertyInfo::FileUrl: return m_fileUrl;
    case PropertyInfo::FileSize: return std::to_string(m_fileSize);
    default: return{ };
    }
}

void CompletedFileInfo::SetElement(int index, const std::string &value)
{
    switch (index)
    {
    case PropertyInfo::DownloadDateTime:
        m_downloadDateTime = value;
        break;
    case PropertyInfo::FileSize:
        SetData(m_fileSize, value);
        break;
    case PropertyInfo::FileUrl:
        m_fileUrl = value;
    default:
        break;
    }
}

void CompletedFileInfo::SetCreatedDateTime()
{
    auto timebase = std::time(nullptr);
    tm timeInfo;

    localtime_s(&timeInfo, &timebase);

    std::ostringstream oss;

    oss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
    m_downloadDateTime = oss.str();
}

