#include "cmgConsole.h"

#include <Windows.h>
//#include <cmgCore/platform/cmgWindowsHeaders.h>


namespace cmg { namespace core { namespace console {
	
	void SetConsoleColor(int foreColor, int backColor)
	{
		WORD combinedColor = 0;
		combinedColor |= foreColor;
		combinedColor |= (backColor << 4);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), combinedColor);
	}

	void EnableUnicode()
	{
		SetConsoleOutputCP(CP_UTF8);
	}

};};};
