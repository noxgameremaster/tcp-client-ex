
#include "headerdata.h"
#include "localbuffer.h"

HeaderData::HeaderData()
    : NetObject()
{
    m_stx = header_stx;
    m_length = 0;
    m_type = 0;
    m_ttx = header_terminal;

    PropertyEntry();
}

HeaderData::~HeaderData()
{ }

void HeaderData::PropertyEntry()
{
    NewProperty<FieldInfo::STX, decltype(m_stx)>(&m_stx);
    NewProperty<FieldInfo::LENGTH, decltype(m_length)>(&m_length);
    NewProperty<FieldInfo::TYPE, decltype(m_type)>(&m_type);
    NewProperty<FieldInfo::ETX, decltype(m_ttx)>(&m_ttx);
}

bool HeaderData::MakeData(std::shared_ptr<LocalBuffer> localbuffer, uint32_t offset)
{
    if (!localbuffer)
        return false;

    uint32_t workpos = offset;

    localbuffer->PeekInc(m_stx, workpos);
    localbuffer->PeekInc(m_length, workpos);
    localbuffer->PeekInc(m_type, workpos);
    localbuffer->Peek(m_ttx, m_length - sizeof(m_ttx));

    return true;
}

size_t HeaderData::FieldLength() const //@brief. 순수 헤더 데이터 만의 크기입니다
{
    return sizeof(m_stx) + sizeof(m_length) + sizeof(m_type) + sizeof(m_ttx);
}
