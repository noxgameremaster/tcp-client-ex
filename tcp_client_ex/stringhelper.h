
#ifndef STRING_HELPER_H__
#define STRING_HELPER_H__

#include <string>
#include <stdexcept>
#include <memory>

namespace _StringHelper
{
	template <class Container>
	inline auto toArray(Container &c)->decltype(&c[0])
	{
		return &c[0];
	}

	/*
	* a simple method
	* this function works converting an any const string to non const string object
	*/
	template <class Container>
	inline auto toArray(const Container &cc)
		->decltype(const_cast<typename Container::value_type *>(&cc[0]))
	{
		return const_cast<Container::value_type *>(cc.data());
	}

	template <class CharT>
	inline CharT *deleteStringConst(const std::basic_string<CharT> &cc)
	{
		return const_cast<CharT *>(cc.data());
	}

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

	namespace helperPrivate
	{
		template <class T>
		inline T strTransferT(T const &t)
		{
			return t;
		}

		inline const char *strTransferT(const std::string &str)
		{
			return str.data();
		}

		template <typename... Args>
		std::string stringFormatPrivate(const std::string &format, Args ...args)
		{
			int length = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;

			if (length <= 0)
			{
				throw std::runtime_error("error - stringFormat length not be 0 or minus!!");
			}
			std::unique_ptr<char[]> pBuffer = std::unique_ptr<char[]>(new char[length]);

			std::snprintf(pBuffer.get(), length, format.c_str(), args...);
			return pBuffer.get();
		}
	}

	template <class... Args>
	std::string stringFormat(const std::string &format, Args ...args)
	{
		return helperPrivate::stringFormatPrivate(format, helperPrivate::strTransferT(args)...);
	}

	template <typename CharT>
	inline void stringLeftTrim(std::basic_string<CharT> &target)
	{
		target.erase(target.begin(), std::find_if(target.begin(), target.end(), [](const CharT &c) { return !std::isspace(c); }));
	}

	template <typename CharT>
	inline void stringRightTrim(std::basic_string<CharT> &target)
	{
		target.erase(std::find_if(target.rbegin(), target.rend(), [](const CharT &c) { return !std::isspace(c); }).base(), target.end());
	}

	template <typename CharT>
	void stringTrim(std::basic_string<CharT> &target)
	{
		stringLeftTrim(target);
		stringRightTrim(target);
	}
};

#endif

