#ifndef _CMG_GRAPHICS_SPRITE_FONT_H_
#define _CMG_GRAPHICS_SPRITE_FONT_H_

#include <cmgCore/smart_ptr/cmg_smart_ptr.h>
#include <cmgGraphics/cmgTexture.h>
#include <cmgCore/io/cmgPath.h>
#include <cmgGraphics/cmgBuiltInFonts.h>


class SpriteFont
{
public:
	//friend class Graphics;
	using sptr = cmg::shared_ptr<SpriteFont>;

public:
	//-----------------------------------------------------------------------------
	// Constructors
	//-----------------------------------------------------------------------------

	SpriteFont();
	~SpriteFont();

	//-----------------------------------------------------------------------------
	// Getters
	//-----------------------------------------------------------------------------

	inline Texture* GetTexture() const
	{
		return m_texture;
	}

	//-----------------------------------------------------------------------------
	// Static Methods
	//-----------------------------------------------------------------------------

	static SpriteFont* LoadSpriteFont(const Path& path,
		int charsPerRow, int charWidth, int charHeight, int charSpacing);
	
	static SpriteFont* LoadBuiltInFont(BuiltInFonts builtInFont);

private:
public:
	Texture* m_texture;
	int m_charWidth;
	int m_charHeight;
	int m_charSpacing;
	int m_charsPerRow;
};


#endif // _CMG_GRAPHICS_SPRITE_FONT_H_