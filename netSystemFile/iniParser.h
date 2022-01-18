
#ifndef INI_PARSER_H__
#define INI_PARSER_H__

#include <list>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

class IniToken;

class IniParser
{
public:
	using tokenType = std::shared_ptr<IniToken>;
	using iniSectionElem = std::map<std::string, std::string>;
	using iniSections = std::map<std::string, iniSectionElem>;

private:
	std::list<uint8_t> m_tokens;
	std::list<tokenType> m_list;
	std::list<tokenType> m_oplist;
	int m_linecount;

	tokenType m_lastsection;

	iniSections m_sectionDict;

public:
	explicit IniParser();
	~IniParser();

private:
	bool addKey(const std::string &sectionkey, const std::string &key, const std::string &value);
	bool addSection(const std::string &sectionkey);

	bool readc(int &c);
	bool unreadc(int c);
	bool nextc(int expect);

	bool readEscapedchar(int &destc);

	bool skipBlock();
	bool skipLine();
	bool doSkipSpace();
	bool skipSpace();
	bool isWhitespace(int c);

	tokenType makeInvalid(const std::string &name);
	tokenType readString();
	tokenType readIdent(int first);

	tokenType doReadToken();
	tokenType opPop();
	tokenType keyPop();
	bool matchKey();
	bool makeSection();
	bool process();

	bool analysis();
	void showAll();

public:
	bool LoadData(const std::vector<uint8_t> &loadvec);

private:
    enum class FindKeyResult
    {
        Found,
        NoSection,
        NoItem
    };
	using findkeyFunctionType = std::function<void(std::string &)>;
    FindKeyResult FindKey(const std::string &sectionkey, const std::string &itemkey, findkeyFunctionType &&action);

public:
	bool GetData(const std::string &sectionkey, const std::string &itemkey, std::string &dest);
	bool SetData(const std::string &sectionkey, const std::string &itemkey, const std::string &itemvalue);

	bool SectionPreorder(std::function<bool(const std::string &)> &&action);

private:

	template <class CharT>
	std::basic_string<CharT> AdjectCharT(const std::basic_string<CharT> &src, CharT tLeft, CharT tRight)
	{
		size_t srcLength = src.size();
		std::basic_string<CharT> tResult(srcLength + 2, 0);

		tResult[0] = tLeft;
		tResult[++srcLength] = tRight;
		std::copy(src.cbegin(), src.cend(), ++tResult.begin());

		return tResult;
	}

	template <class CharT>
	inline std::basic_string<CharT> AdjectCharT(const std::basic_string<CharT> &src, CharT tBoth)
	{
		return AdjectCharT(src, tBoth, tBoth);
	}
};

#endif

