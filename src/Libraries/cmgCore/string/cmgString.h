#ifndef _CMG_CORE_STRING_H_
#define _CMG_CORE_STRING_H_

#include <cmgCore/cmgBase.h>
#include <cmgCore/containers/cmgArray.h>

using String = std::string;

	
namespace string
{
	size_t CharAsciiToWide(wchar_t* destination, const char* source, size_t size);

	// Split string on delimiter into a given container
	uint32 Split(Array<String>& container, const String& str, const String& delim = " ", int32 count = -1);

	// Split string on delimiter and return an Array
	Array<String> Split(const String& str, const String& delim = " ", int32 count = -1);

	// trim from start (in place)
	void TrimLeftIP(String& str);

	// trim from end (in place)
	void TrimRightIP(String& str);

	// trim from both ends (in place)
	void TrimIP(String& str);

	// trim from start (copying)
	String TrimLeft(String str);

	// trim from end (copying)
	String TrimRight(String str);

	// trim from both ends (copying)
	String Trim(String str);
};

/*class String : public std::string
{
public:
	String() {}
	String(const String& copy)
		: std::string(copy)
	{ }
	String(const char* cstr)
		: std::string(cstr)
	{ }
	String(const std::string& str)
		: std::string(str)
	{ }

private:

};*/


#endif // _CMG_CORE_STRING_H_