
#include "reportErrorPacket.h"
#include "packetOrderTable.h"
#include "stringHelper.h"

using namespace _StringHelper;

ReportErrorPacket::ReportErrorPacket()
    : NetPacket()
{
    m_errCodeLow = 0;
    m_errCodeHigh = 0;
    m_errContent.fill(0);
    m_errTitle.fill(0);
}

ReportErrorPacket::~ReportErrorPacket()
{ }

size_t ReportErrorPacket::PacketSize(Mode /*mode*/)
{
    return 0;
}

bool ReportErrorPacket::OnReadPacket()
{
    try
    {
        ReadCtx(m_errCodeLow);
        ReadCtx(m_errCodeHigh);

        for (auto &c : m_errTitle)
            ReadCtx(c);

        for (auto &c : m_errContent)
            ReadCtx(c);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool ReportErrorPacket::OnWritePacket()
{
    return false;
}

uint8_t ReportErrorPacket::GetPacketId() const
{
    return static_cast<uint8_t>(PacketOrderTable<ReportErrorPacket>::GetId());
}

std::list<std::string> ReportErrorPacket::GetErrorInfo()
{
    const std::string noErr = "no error";
    std::list<std::string> errorList;

    errorList.push_back(stringFormat("error code- low:%d, high:%d", m_errCodeLow, m_errCodeHigh));
    errorList.push_back((m_errTitle[0] == 0) ? noErr : m_errTitle.data());
    errorList.push_back((m_errContent[0] == 0) ? noErr : m_errContent.data());

    return errorList;
}