
#include "pch.h"
#include "downloadFileInfo.h"

DownloadFileInfo::DownloadFileInfo()
    : ListElement()
{ }

DownloadFileInfo::~DownloadFileInfo()
{ }

std::string DownloadFileInfo::GetElement(int index)
{
    switch (index)
    {
    case 0: return m_filename;
    case 1: return std::to_string(m_fileSize);
    case 2: return std::to_string(m_downloadBytes);
    default: return{ };
    }
}

void DownloadFileInfo::SetElement(int index, const std::string &value)
{
    switch (index)
    {
    case 0:
        m_filename = value;
        break;
    case 1:
        SetData(m_fileSize, value);
        break;
    case 2:
        SetData(m_downloadBytes, value);
        break;
    default:
        return;
    }
}