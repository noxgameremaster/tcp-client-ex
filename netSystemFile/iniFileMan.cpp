
#include "iniFileMan.h"
#include "iniParser.h"
#include <vector>

IniFileMan::IniFileMan()
	: FileStream()
{
	m_parser = std::make_unique<IniParser>();
	m_parseGetFunction = &IniParser::GetData;
	m_parseSetFunction = &IniParser::SetData;
}

IniFileMan::~IniFileMan()
{ }

bool IniFileMan::ReadIni(const std::string &inifile)
{
	if (!Open(FileStreamInterface::FileOpenMode::ReadOnly, inifile))
		return false;

	uint32_t iFilesize = static_cast<uint32_t>(Filesize());

	std::vector<uint8_t> stream(iFilesize);

	if (!ReadVector(stream))
		return false;

	bool pResult = m_parser->LoadData(stream);

	Close();

	return pResult;
}

bool IniFileMan::writeIniPrivate(const std::string &wrStream)
{
	if (Write(reinterpret_cast<const uint8_t *>(wrStream.c_str()), wrStream.length()))
		return WriteVector({ '\n' });

	return false;
}

bool IniFileMan::WriteIni(const std::string &/*inifile*/)
{
	if (!Open(FileStreamInterface::FileOpenMode::WriteOnly))
		return false;

	m_parser->SectionPreorder([this](const std::string &cs) { return this->writeIniPrivate(cs); });

	Close();
	return true;
}