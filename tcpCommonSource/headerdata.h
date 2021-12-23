
#ifndef HEADER_DATA_H__
#define HEADER_DATA_H__

#include "binarystream.h"
#include <map>

//class LocalBuffer;

class HeaderData : public BinaryStream
{
public:
    static constexpr int header_stx = 0xdeadface;
    static constexpr int header_terminal = 0xfadeface;

private:
    struct HeaderVariantBase
    { };
    struct HeaderDataPimpl;

public:
    enum class FieldInfo
    {
        STX,
        LENGTH,
        PAIR_NUMBER,
        TYPE,
        PACKET_ORDER,
        COMPRESS_TYPE,
        CRYPT_KEY,
        PACKET_CRC,
        MAIN_CMD_TYPE,
        SUB_CMD_TYPE,
        ETX
    };

private:
    std::unique_ptr<HeaderDataPimpl> m_pimpl;
    std::map<FieldInfo, std::unique_ptr<HeaderVariantBase>> m_propertyMap;

    size_t m_headerSizeOnly;
    size_t m_dataOffset;

public:
    HeaderData();
    ~HeaderData() override;

private:
    void PropertyEntry();

    template <FieldInfo fieldId, class Ty>
    struct HeaderVariant : public HeaderVariantBase
    {
        Ty *m_ptr;

    public:
        HeaderVariant(Ty *ptr)
        {
            m_ptr = ptr;
        }
    };

    bool FindProperty(FieldInfo fieldval, HeaderVariantBase *&variant)
    {
        auto propertyIterator = m_propertyMap.find(fieldval);

        if (propertyIterator == m_propertyMap.cend())
            return false;

        variant = propertyIterator->second.get();
        return true;
    }

    template <FieldInfo fieldId, class Ty>
    bool NewProperty(Ty *ptr)
    {
        HeaderVariantBase *variant = nullptr;

        if (FindProperty(fieldId, variant))
            return false;

        m_headerSizeOnly += sizeof(Ty);
        m_propertyMap.emplace(fieldId, std::make_unique<HeaderVariant<fieldId, Ty>>(ptr));
        return true;
    }

public:
    bool MakeData();

    size_t DataSectionOffset() const
    {
        return m_dataOffset;
    }

    size_t FieldLength() const
    {
        return m_headerSizeOnly;
    }

    template <FieldInfo fieldId, class Ty>
    bool GetProperty(Ty &dest)
    {
        HeaderVariantBase *variant = nullptr;

        if (!FindProperty(fieldId, variant))
            return false;

        auto *pProperty = static_cast<HeaderVariant<fieldId, Ty> *>(variant);

        dest = *pProperty->m_ptr;
        return true;
    }

    template <FieldInfo fieldId, class Ty>
    bool SetProperty(const Ty &src)
    {
        HeaderVariantBase *variant = nullptr;

        if (!FindProperty(fieldId, variant))
            return false;

        auto *pProperty = static_cast<HeaderVariant<fieldId, Ty> *>(variant);

        *pProperty->m_ptr = src;
        return true;
    }

    std::vector<uint8_t> ReleaseData();

};

#endif

