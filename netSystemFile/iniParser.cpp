

#include "iniParser.h"
#include "iniToken.h"
#include <iostream>

IniParser::IniParser()
{
	m_linecount = 1;
}

IniParser::~IniParser()
{ }

bool IniParser::addKey(const std::string &sectionkey, const std::string &key, const std::string &value)
{
	auto sectionIterator = m_sectionDict.find(sectionkey);

	if (sectionIterator == m_sectionDict.end())
		return false;

	iniSectionElem &cursection = sectionIterator->second;
	auto keyFindIterator = cursection.find(key);

	if (keyFindIterator != cursection.end())
		keyFindIterator->second = value;
	else
		cursection.emplace(key, value);

	return true;
}

bool IniParser::addSection(const std::string &sectionkey)
{
	auto sectionIterator = m_sectionDict.find(sectionkey);

	if (sectionIterator != m_sectionDict.end())
		return false;
	m_sectionDict.emplace(sectionkey, iniSectionElem());
	return true;
}

bool IniParser::readc(int &c)
{
	if (m_tokens.empty())
		return false;

	c = m_tokens.front();
	m_tokens.pop_front();
	return true;
}

bool IniParser::unreadc(int c)
{
	m_tokens.push_front(c);

	return false;
}

bool IniParser::nextc(int expect)
{
	int c;

	if (!readc(c))
		return false;

	return (c == expect) ? true : unreadc(c);
}

bool IniParser::readEscapedchar(int &destc)
{
	auto pass = [](int &gt, int esc) { gt = esc; return true; };
	int c;

	if (!readc(c))
		return false;

	switch (c) {
	case '\'': case '"': case '?': case '\\':
		return pass(destc, c);
	case 'a': return pass(destc, '\a');
	case 'b': return pass(destc, '\b');
	case 'f': return pass(destc, '\f');
	case 'n': return pass(destc, '\n');
	case 'r': return pass(destc, '\r');
	case 't': return pass(destc, '\t');
	case 'v': return pass(destc, '\v');
	case 'e': return pass(destc, '\033');
	}
	return false;
}

bool IniParser::isWhitespace(int c)
{
	switch (c)
	{
	case ' ': case '\t': case '\f': case '\v':
		return true;
	}
	return false;
}

bool IniParser::skipBlock()
{
	bool maybeEnd = false;

	while (1)
	{
		int c;
		if (!readc(c))
			break;
		if (c == '/' && maybeEnd)
			return true;
		maybeEnd = (c == '*');
	}
	return false;
}

bool IniParser::skipLine()
{
	while (1)
	{
		int c;

		if (!readc(c))
			return false;
		if (c == '\n')
		{
			unreadc(c);
			break;
		}
	}
	return true;
}

bool IniParser::doSkipSpace()
{
	int c;

	if (!readc(c))
		return false;

	if (isWhitespace(c))
		return true;
	if (c == '/')
	{
		if (nextc('*'))
		{
			return skipBlock();
		}
		if (nextc('/'))
		{
			return skipLine();
		}
	}
	return unreadc(c);
}

bool IniParser::skipSpace()
{
	if (!doSkipSpace())
		return false;

	while (doSkipSpace());
	return true;
}

IniParser::tokenType IniParser::makeInvalid(const std::string &name)
{
	return std::make_shared<IniToken>(name, iniTokenType::INVALID);
}

IniParser::tokenType IniParser::readString()
{
	std::list<uint8_t> buffer;
	int c;

	do
	{
		if (!readc(c))
			return makeInvalid("unterminated string");

		if (c == '"')
			break;
		if (c != '\\')
		{
			buffer.push_back(c);
			continue;
		}

		int esc;

		if (!readEscapedchar(esc))
			return makeInvalid("unknown escaped char");
		buffer.push_back(esc);

	}
	while (1);

	return std::make_shared<IniToken>(std::string(buffer.begin(), buffer.end()), iniTokenType::STRING);
}

IniParser::tokenType IniParser::readIdent(int c)
{
	std::list<uint8_t> buffer(1, c);

	do
	{
		if (!readc(c))
			return makeInvalid("undefined behavior:: eof");

		if (isalnum(c) || (c & 0x80) || c == '_' || c == '$')
		{
			buffer.push_back(c);
			continue;
		}
		unreadc(c);

		return std::make_shared<IniToken>(std::string(buffer.begin(), buffer.end()), iniTokenType::IDENTIFIER);
	}
	while (1);
}

IniParser::tokenType IniParser::doReadToken()
{
	if (skipSpace())
		return std::make_shared<IniToken>("space", iniTokenType::SKIP_SPACE);

	int c;

	readc(c);

	switch (c)
	{
	case '\r': return std::make_shared<IniToken>(std::string(1, c), iniTokenType::NOTHING);
	case '\n': return std::make_shared<IniToken>(std::to_string(m_linecount++), iniTokenType::NEW_LINE);
	case '[': return std::make_shared<IniToken>(std::string(1, c), iniTokenType::SEC_ENTRY);
	case ']': return std::make_shared<IniToken>(std::string(1, c), iniTokenType::SEC_EXIT);
	case '"': return readString();
	case '$': case '_': return readIdent(c);
	case '=': return std::make_shared<IniToken>(std::string(1, c), iniTokenType::OP_ASSIGN);
	default:
		if (isalpha(c)) return readIdent(c);
		else return makeInvalid("unknown token:: " + std::to_string(c));
	}
}

bool IniParser::process()
{
	while (m_tokens.size())
	{
		auto &&token = doReadToken();

		switch (token->Type())
		{
		case iniTokenType::INVALID:
			std::cout << "error:: " + token->Name() + ", line::" << std::endl;
			return false;

		case iniTokenType::STRING:
		case iniTokenType::IDENTIFIER:
			m_list.emplace_back(std::move(token));
			break;
		case iniTokenType::SEC_ENTRY:
		case iniTokenType::SEC_EXIT:
		case iniTokenType::OP_ASSIGN:
			m_oplist.emplace_back(std::move(token));
			break;

		default:
			break;
		}
	}

	return true;
}

IniParser::tokenType IniParser::opPop()
{
	if (m_oplist.empty())
		return nullptr;

	auto poptoken = m_oplist.front();
	m_oplist.pop_front();
	return poptoken;
}

IniParser::tokenType IniParser::keyPop()
{
	if (m_list.empty())
		return nullptr;

	auto poptoken = m_list.front();
	m_list.pop_front();
	return poptoken;
}

bool IniParser::matchKey()
{
	auto key = keyPop();
	auto value = keyPop();

	if (!key || !value)
		return false;
	if (key->Type() != iniTokenType::IDENTIFIER)
		return false;
	if (value->Type() != iniTokenType::STRING)
		return false;
	if (!m_lastsection)
		return false;

	return addKey(m_lastsection->Name(), key->Name(), value->Name());
}

bool IniParser::makeSection()
{
	auto entry = opPop();
	auto sectionkey = keyPop();

	if (!entry || !sectionkey)
		return false;
	if (entry->Type() != iniTokenType::SEC_EXIT)
		return false;
	if (sectionkey->Type() != iniTokenType::IDENTIFIER)
		return false;
	if (!addSection(sectionkey->Name()))
		return false;

	m_lastsection = sectionkey;
	return true;
}

bool IniParser::analysis()
{
	while (1)
	{
		auto tok = opPop();

		if (!tok)	//empty
			break;

		switch (tok->Type())
		{
		case iniTokenType::OP_ASSIGN:
			if (!matchKey())
				return false;
			break;
		case iniTokenType::SEC_ENTRY:
			if (!makeSection())
				return false;
		}
	}
	showAll();
	return true;
}

void IniParser::showAll()
{
	for (const auto &sections : m_sectionDict)
	{
		std::cout << sections.first << std::endl;
		for (const auto &item : sections.second)
			std::cout << "key: " << item.first << ", value: " << item.second << std::endl;
	}
}

bool IniParser::LoadData(const std::vector<uint8_t> &loadvec)
{
	m_tokens = std::list<uint8_t>(loadvec.begin(), loadvec.end());

	if (!process())
		return false;
	return analysis();
}

bool IniParser::FindKey(const std::string &sectionkey, const std::string &itemkey, IniParser::findkeyFunctionType &&action)
{
	auto sectionkeyIterator = m_sectionDict.find(sectionkey);

	if (sectionkeyIterator == m_sectionDict.end())
		return false;

	auto &currentSection = sectionkeyIterator->second;
	auto keyIterator = currentSection.find(itemkey);

	if (keyIterator == currentSection.end())
		return false;
	action(keyIterator->second);

	return true;
}

bool IniParser::GetData(const std::string &sectionkey, const std::string &itemkey, std::string &dest)
{
	return FindKey(sectionkey, itemkey, [&dest](std::string &s) { dest = s; });
}

bool IniParser::SetData(const std::string &sectionkey, const std::string &itemkey, const std::string &itemvalue)
{
	return FindKey(sectionkey, itemkey, [&itemvalue](std::string &s) { s = itemvalue; });
}

bool IniParser::SectionPreorder(std::function<bool(const std::string &)> &&action)
{
	for (const auto &section : m_sectionDict)
	{
		if (!action(AdjectCharT(section.first, '[', ']')))
			return false;

		const auto &valueSection = section.second;

		for (const auto &elem : valueSection)
		{
			if (!action(elem.first + '=' + AdjectCharT(elem.second, '"')))
				return false;
		}
	}
	return true;
}