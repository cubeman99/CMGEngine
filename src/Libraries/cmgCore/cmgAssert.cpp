#include "cmgAssert.h"

#ifdef _WIN32
#include <Windows.h>
//#include <cmgCore/platform/cmgWindowsHeaders.h>
#endif


namespace cmg { namespace core { namespace assert {

	bool AlwaysReturnFalse()
	{
		return false;
	}

	bool IsInDebugSession()
	{
		#ifdef _WIN32
			return (IsDebuggerPresent() == TRUE);
		#endif
		return false;
	}
	
	void HardExit(int exitCode)
	{
		exit(exitCode);
	}

};};};

