
#ifndef INI_TOKEN_H__
#define INI_TOKEN_H__

#include <string>

enum class iniTokenType
{
	INVALID,
	IDENTIFIER,
	STRING,
	SKIP_SPACE,
	OP_ASSIGN,
	NEW_LINE,
	SEC_ENTRY,
	SEC_EXIT,
	NOTHING
};

class IniToken
{
private:
	std::string m_name;
	iniTokenType m_type;

public:
	explicit IniToken(const std::string &tokenname, iniTokenType toktype);

	~IniToken();

public:
	iniTokenType Type() const {
		return m_type;
	}
	std::string Name() const {
		return m_name;
	}

};

#endif



