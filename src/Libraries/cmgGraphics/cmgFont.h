#ifndef _CMG_GRAPHICS_FONT_H_
#define _CMG_GRAPHICS_FONT_H_

#include <cmgGraphics/cmgTexture.h>
#include <cmgCore/io/cmgPath.h>
#include <cmgMath/types/cmgVector2.h>
#include <cmgGraphics/cmgBuiltInFonts.h>


/*
(-y)         =================
.             GLYPH METRICS 
/|\          ================= 
|   minX
|    .          .
|    .   width  .
|    <---------->
|    .          .
|    +----------+ . . . . . .
|    |  .--./)  |           |
|    | /.''\\   |           |
|    | |  | |   |           |
|    | \`-' /   |           |
|    | /("''    |           |
(-x) <---@----+-\-'---.--+-----@-->  | height
|    |  /'""'.\ |     .     |
|    | ||     |||     .     | 
|    | \'. __// |     .     |
|    |  `'---'  |     .     |
|    +----------+ . . . . . . . . minY
|                     . 
|                     . 
|-------------------->.
|        advance                 
\|/
'
(+y)
*/


//-----------------------------------------------------------------------------
// Glyph
//-----------------------------------------------------------------------------

// A Glyph represents the metrics and image
// offset for a single character code.
class Glyph
{
	friend class Font;

public:
	inline bool HasImage() const { return (m_width > 0 && m_height > 0); }
	inline int32 GetAdvance() const { return m_advance; }
	inline int32 GetMinX() const { return m_minX; }
	inline int32 GetMinY() const { return m_minY; }
	inline int32 GetMaxX() const { return m_minX + m_width; }
	inline int32 GetMaxY() const { return m_minY + m_height; }
	inline int32 GetWidth() const { return m_width; }
	inline int32 GetHeight() const { return m_height; }
	inline int32 GetSourceX() const { return m_sourceX; }
	inline int32 GetSourceY() const { return m_sourceY; }

private:
	Glyph() {}
	void Init(void* ftGlyphPtr);

	int32 m_advance; // Amount to move the pen position horizontally after drawing this glyph
	int32 m_minX = 0; // Image x-draw-offset from pen position
	int32 m_minY = 0;  // Image y-draw-offset from pen position
	int32 m_width; // Width of glyph image
	int32 m_height; // Height of glyph image
	int32 m_sourceX = 0; // Image X-offset in the font's glyph atlas texture
	int32 m_sourceY = 0; // Image Y-offset in the font's glyph atlas texture
};



//-----------------------------------------------------------------------------
// Font
//-----------------------------------------------------------------------------

struct CharacterRegion
{
	uint32 begin; // First character code in the region
	uint32 length; // Number of characters in the region
};


// A fixed-size bitmap font that stores glyph images
// for a region of character codes.
class Font : public cmg::ResourceImpl<Font>
{
public:
	using sptr = cmg::shared_ptr<Font>;

public:
	Font();
	~Font();

	// Accessors
	const Glyph* GetGlyph(char c) const;
	const Glyph* GetGlyph(uint32 charCode) const;
	bool IsCharDefined(uint32 charCode) const;
	inline int32 GetSize() const { return m_size; }
	inline const String& GetFamilyName() const { return m_familyName; }
	inline const String& GetStyleName() const { return m_styleName; }
	inline const Texture* GetGlyphTexture()	const { return m_glyphAtlasTexture; }
	inline Texture* GetGlyphTexture() { return m_glyphAtlasTexture; }

	// FreeType library
	static bool InitFreeTypeLibrary();
	static void QuitFreeTypeLibrary();

	// Font loading
	static Error LoadFont(Font*& outFont, const Path& path, int32 size,
		uint32 charRegionBegin, uint32 charRegionEnd);
	static Error LoadSpriteFont(Font*& outFont, const Path& path,
		uint32 charsPerRow, uint32 charWidth, uint32 charHeight,
		uint32 charSpacing = 0);
	static Error LoadBuiltInFont(Font*& outFont, BuiltInFonts builtInFont);

private:
	Font(Texture* texture);
	Font(void* ftFace, int32 size, uint32 charRegionBegin, uint32 charRegionEnd);

	// Prevent copying
	Font(const Font& other) {}
	void operator=(const Font& other) {}

	String m_familyName = "";
	String m_styleName = "";
	int32 m_size = 1;
	Glyph* m_glyphs = nullptr;
	Texture* m_glyphAtlasTexture = nullptr;
	CharacterRegion m_charRegion;
};



#endif // _CMG_GRAPHICS_FONT_H_