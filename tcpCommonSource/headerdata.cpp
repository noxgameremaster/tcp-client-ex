
#include "headerdata.h"
//#include "localbuffer.h"

struct HeaderData::HeaderDataPimpl
{
    int m_stx;
    size_t m_length;
    int m_pairNumber;   //unused
    char m_type;
    int m_packetOrder;  //udp only
    char m_compressType; //unused
    int m_cryptKey; //un
    int m_crc;  //un
    char m_mainCmdType;
    char m_subCmdType;
    int m_etx;
};

HeaderData::HeaderData()
    : BinaryStream(sizeof(HeaderDataPimpl)), m_pimpl(std::make_unique<HeaderDataPimpl>())
{
    m_headerSizeOnly = 0;
    m_dataOffset = 0;

    m_pimpl->m_stx = header_stx;
    m_pimpl->m_length = 0;
    m_pimpl->m_pairNumber = 1;
    m_pimpl->m_type = 0;
    m_pimpl->m_packetOrder = -1;
    m_pimpl->m_compressType = 0;
    m_pimpl->m_cryptKey = 0;
    m_pimpl->m_crc = 0;
    m_pimpl->m_mainCmdType = 0;
    m_pimpl->m_subCmdType = 0;
    m_pimpl->m_etx = header_terminal;

    PropertyEntry();
}

HeaderData::~HeaderData()
{ }

void HeaderData::PropertyEntry()
{
    NewProperty<FieldInfo::STX>(&m_pimpl->m_stx);
    NewProperty<FieldInfo::LENGTH>(&m_pimpl->m_length);
    NewProperty<FieldInfo::PAIR_NUMBER>(&m_pimpl->m_pairNumber);
    NewProperty<FieldInfo::TYPE>(&m_pimpl->m_type);
    NewProperty<FieldInfo::PACKET_ORDER>(&m_pimpl->m_packetOrder);
    NewProperty<FieldInfo::COMPRESS_TYPE>(&m_pimpl->m_compressType);
    NewProperty<FieldInfo::CRYPT_KEY>(&m_pimpl->m_cryptKey);
    NewProperty<FieldInfo::PACKET_CRC>(&m_pimpl->m_crc);
    NewProperty<FieldInfo::MAIN_CMD_TYPE>(&m_pimpl->m_mainCmdType);
    NewProperty<FieldInfo::SUB_CMD_TYPE>(&m_pimpl->m_subCmdType);
    m_dataOffset = m_headerSizeOnly;
    NewProperty<FieldInfo::ETX>(&m_pimpl->m_etx);
}

bool HeaderData::MakeData()
{
    try
    {
        ReadCtx(m_pimpl->m_stx);
        ReadCtx(m_pimpl->m_length);
        ReadCtx(m_pimpl->m_pairNumber);
        ReadCtx(m_pimpl->m_type);
        ReadCtx(m_pimpl->m_packetOrder);
        ReadCtx(m_pimpl->m_compressType);
        ReadCtx(m_pimpl->m_cryptKey);
        ReadCtx(m_pimpl->m_crc);
        ReadCtx(m_pimpl->m_mainCmdType);
        ReadCtx(m_pimpl->m_subCmdType);
    }
    catch (const bool &fail)
    {
        return fail;
    }    
    return true;
}

std::vector<uint8_t> HeaderData::ReleaseData()
{
    BufferResize(m_dataOffset);

    try
    {
        WriteCtx(m_pimpl->m_stx);
        WriteCtx(m_pimpl->m_length);
        WriteCtx(m_pimpl->m_pairNumber);
        WriteCtx(m_pimpl->m_type);
        WriteCtx(m_pimpl->m_packetOrder);
        WriteCtx(m_pimpl->m_compressType);
        WriteCtx(m_pimpl->m_cryptKey);
        WriteCtx(m_pimpl->m_crc);
        WriteCtx(m_pimpl->m_mainCmdType);
        WriteCtx(m_pimpl->m_subCmdType);
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