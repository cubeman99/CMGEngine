#include "cmgString.h"
#include <algorithm> 
#include <cctype>
#include <locale>

namespace string
{
	
	size_t CharAsciiToWide(wchar_t* destination, const char* source, size_t size)
	{
		size_t retSize;
		::mbstowcs_s(&retSize, destination, size, source, size);
		return retSize;
	}


	// Split string on delimiter into a given container
	uint32 Split(Array<String>& container, const String& str, const String& delim, int32 count)
	{
		std::size_t current, previous = 0;
		std::size_t delimLength = delim.length();
		current = str.find(delim);
		uint32 found = 0;
		while (current != std::string::npos)
		{
			if (current > previous)
			{
				container.push_back(str.substr(previous, current - previous));
				found++;
				if (count > 0 && (int32) found == count)
					return found;
			}
			previous = current + delimLength;
			current = str.find(delim, previous);
		}
		if (previous < str.length())
		{
			container.push_back(str.substr(previous, current - previous));
			found++;
		}
		return found;
	}

	// Split string on delimiter and return an Array
	Array<String> Split(const String& str, const String& delim, int32 count)
	{
		Array<String> result;
		Split(result, str, delim, count);
		return result;
	}

	// trim from start (in place)
	void TrimLeftIP(String& str)
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
			return !std::isspace(ch);
		}));
	}

	// trim from end (in place)
	void TrimRightIP(String& str)
	{
		str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), str.end());
	}

	// trim from both ends (in place)
	void TrimIP(String& str)
	{
		TrimLeftIP(str);
		TrimRightIP(str);
	}

	// trim from start (copying)
	String TrimLeft(String str)
	{
		TrimLeftIP(str);
		return str;
	}

	// trim from end (copying)
	String TrimRight(String str)
	{
		TrimRightIP(str);
		return str;
	}

	// trim from both ends (copying)
	String Trim(String str)
	{
		TrimIP(str);
		return str;
	}
};

