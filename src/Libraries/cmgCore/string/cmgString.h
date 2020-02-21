#ifndef _CMG_CORE_STRING_H_
#define _CMG_CORE_STRING_H_

#include <cmgCore/cmgBase.h>
#include <cmgCore/containers/cmgArray.h>
#include <algorithm> 
#include <cctype>
#include <locale>

using String = std::string;
using StringU16 = std::u16string;
	
namespace cmg { namespace string {

	size_t CharAsciiToWide(wchar_t* destination, const char* source, size_t size);

	// Return true if a character is whitespace
	template<class T_Char>
	bool IsSpace(T_Char c);
	
	// Split string on delimiter into a given container
	uint32 Split(Array<String>& container, const String& str, const String& delim = " ", int32 count = -1);

	// Split string on delimiter and return an Array
	Array<String> Split(const String& str, const String& delim = " ", int32 count = -1);

	// trim from start (in place)
	template <class T_String>
	void TrimLeftIP(T_String& str);

	// trim from end (in place)
	template <class T_String>
	void TrimRightIP(T_String& str);

	// trim from both ends (in place)
	template <class T_String>
	void TrimIP(T_String& str);

	// trim from start (copying)
	template <class T_String>
	T_String TrimLeft(T_String str);

	// trim from end (copying)
	template <class T_String>
	T_String TrimRight(T_String str);

	// trim from both ends (copying)
	template <class T_String>
	T_String Trim(T_String str);

	// Convert to lowercase (in place)
	template <class T_String>
	void ToLowerIP(T_String& str);

	// Return as lowercase
	template <class T_String>
	T_String ToLower(T_String str);

	// Convert to uppercase (in place)
	template <class T_String>
	void ToUpperIP(T_String& str);

	// Return as uppercase
	template <class T_String>
	T_String ToUpper(T_String str);

	template<class T_String>
	bool EndsWith(T_String str, T_String ending);
	template<class T_String>
	bool BeginsWith(T_String str, T_String suffix);

	template<class T_String, class T_Char>
	uint32 ReplaceAll(T_String& str, const T_Char* find, const T_Char* replace);
	template<class T_String>
	uint32 ReplaceAll(T_String& str, const T_String& find, const T_String& replace);



	// Return true if a character is whitespace
	template<class T_Char>
	bool IsSpace(T_Char c)
	{
		return (c == (T_Char) ' ' || c == (T_Char) '\t' ||
			c == (T_Char) '\n' || c == (T_Char) '\r');
	}

	// Convert to lowercase (in place)
	template<class T_String>
	void ToLowerIP(T_String & str)
	{
		std::locale loc;
		std::transform(str.begin(), str.end(), str.begin(), [&](int c) {
			return std::tolower(c, loc);
		});
	}

	// trim from start (in place)
	template<class T_String>
	void TrimLeftIP(T_String& str)
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(),
			[](T_String::value_type ch)
		{
			return !IsSpace(ch);
		}));
	}

	// trim from end (in place)
	template<class T_String>
	void TrimRightIP(T_String& str)
	{
		str.erase(std::find_if(str.rbegin(), str.rend(),
			[](T_String::value_type ch)
		{
			return !IsSpace(ch);
		}).base(), str.end());
	}

	// trim from both ends (in place)
	template<class T_String>
	void TrimIP(T_String& str)
	{
		TrimLeftIP(str);
		TrimRightIP(str);
	}

	// trim from start (copying)
	template<class T_String>
	T_String TrimLeft(T_String str)
	{
		TrimLeftIP(str);
		return str;
	}

	// trim from end (copying)
	template<class T_String>
	T_String TrimRight(T_String str)
	{
		TrimRightIP(str);
		return str;
	}

	// trim from both ends (copying)
	template<class T_String>
	T_String Trim(T_String str)
	{
		TrimIP(str);
		return str;
	}


	template<class T_String>
	T_String ToLower(T_String str)
	{
		ToLowerIP(str);
		return str;
	}

	template<class T_String>
	void ToUpperIP(T_String & str)
	{
		std::locale loc;
		std::transform(str.begin(), str.end(), str.begin(), [&](int c) {
			return std::toupper(c, loc);
		});
	}
	template<class T_String>
	T_String ToUpper(T_String str)
	{
		ToUpperIP(str);
		return str;
	}

	template<class T_String>
	bool EndsWith(T_String str, T_String ending)
	{
		if (ending.length() > str.length())
			return false;
		auto it = str.begin() + (str.length() - ending.length());
		auto it2 = ending.begin();
		for (; it != str.end(); ++it, ++it2)
		{
			if (*it != *it2)
				return false;
		}
		return true;
	}

	template<class T_String>
	bool BeginsWith(T_String str, T_String beginning)
	{
		if (beginning.length() > str.length())
			return false;
		auto it2 = str.begin();
		for (auto it = beginning.begin(); it != beginning.end(); ++it, ++it2)
		{
			if (*it != *it2)
				return false;
		}
		return true;
	}

	template<class T_String, class T_Char>
	uint32 ReplaceAll(T_String& str, const T_Char* find, const T_Char* replace)
	{
		return ReplaceAll(str, T_String(find), T_String(replace));
	}

	template<class T_String>
	uint32 ReplaceAll(T_String& str, const T_String& find, const T_String& replace)
	{
		uint32 count = 0;
		uint32 index = 0;
		uint32 fromLength = find.length();
		uint32 toLength = replace.length();
		while (true)
		{
			// Locate the substring to replace
			index = str.find(find, index);
			if (index == std::string::npos)
				break;

			// Make the replacement
			str.replace(index, fromLength, replace);

			// Advance index forward so the next iteration doesn't pick it up
			index += toLength;
			count++;
		}
		return count;
	}

}

}


#endif // _CMG_CORE_STRING_H_