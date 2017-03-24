#ifndef _SPRITE_FONT_H_
#define _SPRITE_FONT_H_


#include <cmgGraphics/cmg_graphics.h>


class SpriteFont
{
	friend class Graphics;

public:
	SpriteFont();
	SpriteFont(Texture* texture);
	~SpriteFont();

	inline Texture* GetTexture() const { return m_texture; }

	static SpriteFont* LoadFont(const Path& path, int charsPerRow, int charWidth, int charHeight, int charSpacing);


private:
public:
	Texture* m_texture;
	int m_charWidth;
	int m_charHeight;
	int m_charSpacing;
	int m_charsPerRow;
};


#endif // _SPRITE_FONT_H_