
#include "serverfile.h"
#include <fstream>

struct ServerFile::FileParams
{
    std::string m_filename;
    binary_ifstream m_binStream;
};

ServerFile::ServerFile(const std::string &filename)
{
    m_params = std::make_unique<FileParams>();
    if (filename.size())
        Open(filename);
}

ServerFile::~ServerFile()
{ }

bool ServerFile::ReadImpl(uint8_t *stream, const size_t &readSize, size_t &readReal)
{
    if (!m_params->m_binStream)
        return false;

    m_params->m_binStream.read(stream, readSize);
    size_t readBytes = static_cast<size_t>(m_params->m_binStream.gcount());

    if (!readBytes)
        return false;

    readReal = readBytes;
    return true;
}

bool ServerFile::Open(const std::string &filename)
{
    m_params->m_binStream = binary_ifstream(filename, binary_ifstream::binary);

    if (m_params->m_binStream)
    {
        m_params->m_filename = filename;
        return true;
    }
    return false;
}

std::string ServerFile::FileName() const
{
    return m_params->m_filename;
}

