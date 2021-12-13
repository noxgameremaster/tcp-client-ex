
#ifndef SERVER_FILE_H__
#define SERVER_FILE_H__

#include <string>
#include <memory>

class ServerFile
{
    using binary_ifstream = std::basic_ifstream<uint8_t, std::char_traits<uint8_t>>;
    struct FileParams;
private:
    std::unique_ptr<FileParams> m_params;
    
public:
    ServerFile(const std::string &filename);
    ~ServerFile();

private:
    bool ReadImpl(uint8_t *dest, const size_t &readSize, size_t &readReal);

public:
    bool Open(const std::string &fileName);
    template <class Container>
    bool Read(Container &destCont)
    {
        size_t real = 0;

        if (!ReadImpl(destCont.data(), destCont.size(), real))
            return false;
        
        if (destCont.size() != real && real)
            destCont.resize(real);
        return true;
    }
    std::string FileName() const;
};

#endif

