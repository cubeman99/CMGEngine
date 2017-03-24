#ifndef _CMG_CORE_STRING_H_
#define _CMG_CORE_STRING_H_

#include <cmgCore/cmgBase.h>
#include <vector>

	
namespace string
{
	size_t CharAsciiToWide(wchar_t* destination, const char* source, size_t size);
};

typedef std::string String;

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