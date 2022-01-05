

#ifndef FILE_ENGINE_H__
#define FILE_ENGINE_H__

#include <string>
#include <map>
#include <vector>

//파일 처리로직- native, cppstyle, posix 방식의 기능을 모두 포함함

class FileEngine
{
public:
    using winHandle = void *;

private:
    std::string m_filename;
    int m_openMode;
    winHandle m_hMap;
    winHandle m_hFile;
    std::map<uint8_t *, uint32_t> m_maps;
    uint32_t m_iLastBuffLength;

public:
    explicit FileEngine(const std::string &filename);
    virtual ~FileEngine();

    bool Opened() const;
    inline int Openmode() const;

    inline std::string Filename() const;

private:
    virtual bool openfile(int openmode) = 0;
    virtual bool closefile() = 0;

private:
    virtual bool readfile(uint8_t *pData, uint32_t length) = 0;
    virtual bool writefile(const uint8_t *pData, uint32_t length) = 0;

public:
    bool ReadWithVector(std::vector<uint8_t> &readvec);
    bool WriteWithVector(const std::vector<uint8_t> &srcvec);

private:
    inline bool isValidFileHandle() const;
    bool deinitMaphandle();

    bool create(uint32_t accessmode, uint32_t dwCreateFlag);
    bool reset();

    bool mapp(uint64_t offset, uint64_t size, uint8_t *&destmapAddr);
    bool unmapp(uint8_t *ptr);

public:
    bool Open(int openmode);
    bool Close();

    bool Read(uint8_t *pData, uint32_t length, uint32_t *iReadc);
    bool Write(const uint8_t *pData, uint32_t length, uint32_t *iWrtc);
    bool Mapping(uint64_t offset, uint64_t size, uint8_t *&destptr);
    bool Unmapping(uint8_t *ptr);
    bool GetMapExtra(uint8_t *pAddr, uint32_t &iExtra);
    virtual bool Filesize(uint64_t &getsize);
    virtual bool FileCopy(const std::string &copiedFilename);

protected:
    void setBufferLength(uint32_t length);

    const uint32_t m_maxBlocksize;
};


bool FileEngine::isValidFileHandle() const
{
    return m_hMap != nullptr;
}

int FileEngine::Openmode() const
{
    return static_cast<int>(m_openMode);
}

std::string FileEngine::Filename() const
{
    return m_filename;
}

#endif

