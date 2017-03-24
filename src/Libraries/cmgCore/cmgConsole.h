#ifndef _CMG_CONSOLE_H_
#define _CMG_CONSOLE_H_

namespace cmg { namespace core { namespace console {
	
	// Console colors.
	namespace p_color
	{
		enum
		{
			black			= 0, background_default = 0,
			dark_blue		= 1, navy	= 1,
			dark_green		= 2,
			dark_cyan		= 3, teal	= 3,
			dark_red		= 4, maroon	= 4,
			dark_magenta	= 5, purple	= 5, dark_pink = 5,
			dark_yellow		= 6, olive	= 6,
			light_gray		= 7,
			gray			= 8,
			blue			= 9,
			green			= 10,
			cyan			= 11,
			red				= 12,
			magenta			= 13, pink	= 13,
			yellow			= 14,
			white			= 15
		};
	};

	// Set the foreground and background colors of the console.
	void SetConsoleColor(int foreColor, int backColor);

};};};

namespace cmg { namespace core {
	namespace console_color = console::p_color;
};};

#endif // _CMG_CONSOLE_H_