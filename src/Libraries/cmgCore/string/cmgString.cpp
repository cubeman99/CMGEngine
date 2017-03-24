#include "cmgString.h"




namespace string
{
	
	size_t CharAsciiToWide(wchar_t* destination, const char* source, size_t size)
	{
		size_t retSize;
		::mbstowcs_s(&retSize, destination, size, source, size);
		return retSize;
	}

};

