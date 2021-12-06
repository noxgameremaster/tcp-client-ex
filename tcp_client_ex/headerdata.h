
#ifndef HEADER_DATA_H__
#define HEADER_DATA_H__

#include "netobject.h"
#include <map>

class LocalBuffer;

class HeaderData : public NetObject
{
    static constexpr int header_stx = 0xdeadface;
    static constexpr int header_terminal = 0xdeadface;

    struct HeaderVariantBase
    { };

public:
    enum class FieldInfo
    {
        STX,
        LENGTH,
        TYPE,
        ETX
    };

private:
    int m_stx;
    int m_length;
    char m_type;
    int m_ttx;
    std::map<FieldInfo, std::unique_ptr<HeaderVariantBase>> m_propertyMap;

public:
    HeaderData();
    ~HeaderData();

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

        m_propertyMap.emplace(fieldId, std::make_unique<HeaderVariant<fieldId, Ty>>(ptr));
        return true;
    }

public:
    bool MakeData(std::shared_ptr<LocalBuffer> localbuffer, uint32_t offset);

    size_t DataSectionOffset() const
    {
        return sizeof(m_stx) + sizeof(m_length) + sizeof(m_type);
    }

    size_t FieldLength() const;

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

};

#endif

