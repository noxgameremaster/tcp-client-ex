
#include "iniToken.h"

IniToken::IniToken(const std::string &tokenname, iniTokenType toktype)
{
	m_name = tokenname;
	m_type = toktype;
}

IniToken::~IniToken()
{ }

