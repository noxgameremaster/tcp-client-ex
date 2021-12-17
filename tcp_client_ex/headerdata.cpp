
#include "headerdata.h"
#include "localbuffer.h"

HeaderData::HeaderData()
    : BinaryStream(64)
{
    m_headerSizeOnly = 0;
    m_dataOffset = 0;

    m_stx = header_stx;
    m_length = 0;
    m_pairNumber = 1;
    m_type = 0;
    m_packetOrder = -1;
    m_compressType = 0;
    m_cryptKey = 0;
    m_crc = 0;
    m_mainCmdType = 0;
    m_subCmdType = 0;
    m_etx = header_terminal;

    PropertyEntry();
}

HeaderData::~HeaderData()
{ }

void HeaderData::PropertyEntry()
{
    NewProperty<FieldInfo::STX>(&m_stx);
    NewProperty<FieldInfo::LENGTH>(&m_length);
    NewProperty<FieldInfo::PAIR_NUMBER>(&m_pairNumber);
    NewProperty<FieldInfo::TYPE>(&m_type);
    NewProperty<FieldInfo::PACKET_ORDER>(&m_packetOrder);
    NewProperty<FieldInfo::COMPRESS_TYPE>(&m_compressType);
    NewProperty<FieldInfo::CRYPT_KEY>(&m_cryptKey);
    NewProperty<FieldInfo::PACKET_CRC>(&m_crc);
    NewProperty<FieldInfo::MAIN_CMD_TYPE>(&m_mainCmdType);
    NewProperty<FieldInfo::SUB_CMD_TYPE>(&m_subCmdType);
    m_dataOffset = m_headerSizeOnly;
    NewProperty<FieldInfo::ETX>(&m_etx);
}

bool HeaderData::MakeData(std::shared_ptr<LocalBuffer> localbuffer, uint32_t offset)
{
    if (!localbuffer)
        return false;

    uint32_t workpos = offset;

    localbuffer->PeekInc(m_stx, workpos);
    localbuffer->PeekInc(m_length, workpos);
    localbuffer->PeekInc(m_pairNumber, workpos);
    localbuffer->PeekInc(m_type, workpos);
    localbuffer->PeekInc(m_packetOrder, workpos);
    localbuffer->PeekInc(m_compressType, workpos);
    localbuffer->PeekInc(m_cryptKey, workpos);
    localbuffer->PeekInc(m_crc, workpos);
    localbuffer->PeekInc(m_mainCmdType, workpos);
    localbuffer->PeekInc(m_subCmdType, workpos);
    return localbuffer->Peek(m_etx, m_length - sizeof(m_etx));
}

std::vector<uint8_t> HeaderData::ReleaseData()
{
    BufferResize(m_dataOffset);

    try
    {
        WriteCtx(m_stx);
        WriteCtx(m_length);
        WriteCtx(m_pairNumber);
        WriteCtx(m_type);
        WriteCtx(m_packetOrder);
        WriteCtx(m_compressType);
        WriteCtx(m_cryptKey);
        WriteCtx(m_crc);
        WriteCtx(m_mainCmdType);
        WriteCtx(m_subCmdType);
    }
    catch (const bool &)
    {
        return {};
    }
    uint8_t *stream = nullptr;
    size_t length = 0;

    GetStreamInfo(stream, length);
    return std::vector<uint8_t>(stream, stream + length);
}