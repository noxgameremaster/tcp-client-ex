
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
    case PropertyInfo::FileName: return m_filename;
    case PropertyInfo::FileSize: return std::to_string(m_fileSize);
    case PropertyInfo::DownloadBytes: return std::to_string(m_downloadBytes);
    case PropertyInfo::SavePath: return m_savepath;
    default: return{ };
    }
}

void DownloadFileInfo::SetElement(int index, const std::string &value)
{
    switch (index)
    {
    case PropertyInfo::FileName:
        m_filename = value;
        break;
    case PropertyInfo::FileSize:
        SetData(m_fileSize, value);
        break;
    case PropertyInfo::DownloadBytes:
        SetData(m_downloadBytes, value);
        break;
    case PropertyInfo::SavePath:
        m_savepath = value;
        break;
    default:
        return;
    }
}

void DownloadFileInfo::SetFileInfo(const std::string &filename, const std::string &savepath, const size_t size, const size_t downloadbytes)
{
    m_savepath = savepath;
    m_filename = filename;
    m_fileSize = size;
    m_downloadBytes = downloadbytes;
}

bool DownloadFileInfo::Clone(const ListElement *other)
{
    if (!ListElement::Clone(other))
        return false;

    const DownloadFileInfo *castedOther = static_cast<const DownloadFileInfo *>(other);

    m_filename = castedOther->m_filename;
    m_fileSize = castedOther->m_fileSize;
    m_downloadBytes = castedOther->m_downloadBytes;
    m_savepath = castedOther->m_savepath;
    return true;
}

