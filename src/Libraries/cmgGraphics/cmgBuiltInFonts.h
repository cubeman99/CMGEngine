#ifndef _CMG_GRAPHICS_BUILT_IN_FONTS_H_
#define _CMG_GRAPHICS_BUILT_IN_FONTS_H_

enum class BuiltInFonts
{
	FONT_CONSOLE = 0,
};



struct FontConsoleBitmap
{
	static const unsigned int WIDTH = 128;
	static const unsigned int HEIGHT = 192;
	static const unsigned int CHARS_PER_ROW = 16;
	static const unsigned int CHAR_WIDTH = 8;
	static const unsigned int CHAR_HEIGHT = 12;
	static const unsigned int CHAR_SPACING = 0;
	static const unsigned char BITMAP_DATA[WIDTH * HEIGHT];
};

#endif // _CMG_GRAPHICS_BUILT_IN_FONTS_H_