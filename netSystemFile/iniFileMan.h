
#ifndef INI_FILE_MAN_H__
#define INI_FILE_MAN_H__

#include "filestream.h"
#include <sstream>
#include <iostream>

class IniParser;

class IniFileMan : public FileStream
{
private:
	std::unique_ptr<IniParser> m_parser;
	using parserGetFunctionType = bool(IniParser:: *)(const std::string &, const std::string &, std::string &);
	parserGetFunctionType m_parseGetFunction;
	using parserSetFunctionType = bool(IniParser:: *)(const std::string &, const std::string &, const std::string &);
	parserSetFunctionType m_parseSetFunction;

public:
	explicit IniFileMan();
	virtual ~IniFileMan() override;

	bool ReadIni(const std::string &inifile);

private:
	template <typename T>
	inline bool setdata(const std::string &value, T &dst)
	{
		char c;
		std::basic_istringstream<char> is{ value };
		T result;
		if ((is >> std::boolalpha >> result) && !(is >> c))
		{
			dst = result;
			return true;
		}
		else
			return false;
	}

	template <>
	inline bool setdata(const std::string &value, std::string &dst)
	{
		dst = value;
		return true;
	}

public:
	template <typename T>
	bool GetItemValue(const std::string &sectionkey, const std::string &itemkey, T &dest)
	{
		std::string getstr;

		if (!((((m_parser.get())->*m_parseGetFunction)(sectionkey, itemkey, getstr))))
			return false;

		return setdata(getstr, dest);
	}

private:
	template <typename T>
	std::string makeStringdata(const T &value)
	{
		return std::to_string(value);
	}

	template <>
	std::string makeStringdata(const std::string &value)
	{
		return value;
	}

	bool writeIniPrivate(const std::string &wrStream);

public:
	template <typename T>
	bool SetItemValue(const std::string &sectionkey, const std::string &itemkey, const T &value)
	{
		if (!(((m_parser.get())->*m_parseSetFunction)(sectionkey, itemkey, makeStringdata(value))))
			return false;
		return true;
	}

	bool WriteIni(const std::string &inifile = {});
};

#endif

