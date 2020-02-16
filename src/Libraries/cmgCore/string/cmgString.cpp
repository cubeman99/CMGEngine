#include "cmgString.h"

namespace cmg { namespace string {
	
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

	/*
	void ToLowerIP(String& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	}

	String ToLower(String str)
	{
		ToLowerIP(str);
		return str;
	}

	void ToUpperIP(String& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	}

	String ToUpper(String str)
	{
		ToUpperIP(str);
		return str;
	}
	*/
}}

