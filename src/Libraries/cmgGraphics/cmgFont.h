#ifndef _CMG_GRAPHICS_FONT_H_
#define _CMG_GRAPHICS_FONT_H_

#include <cmgCore/smart_ptr/cmg_smart_ptr.h>
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
	inline int GetAdvance() const { return m_advance; }
	inline int GetMinX() const { return m_minX; }
	inline int GetMinY() const { return m_minY; }
	inline int GetMaxX() const { return m_minX + m_width; }
	inline int GetMaxY() const { return m_minY + m_height; }
	inline int GetWidth() const { return m_width; }
	inline int GetHeight() const { return m_height; }
	inline int GetSourceX() const { return m_sourceX; }
	inline int GetSourceY() const { return m_sourceY; }

private:
	Glyph() {}
	void Init(void* ftGlyphPtr);

	int m_advance; // Amount to move the pen position after drawing this glyph
	int m_minX; // Image x-draw-offset from pen position
	int m_minY;  // Image y-draw-offset from pen position
	int m_width; // Width of glyph image
	int m_height; // Height of glyph image.
	int m_sourceX; // Image X-offset in the font's glyph atlas texture
	int m_sourceY; // Image Y-offset in the font's glyph atlas texture
	unsigned long m_charcode;
};



//-----------------------------------------------------------------------------
// Font
//-----------------------------------------------------------------------------

struct CharacterRegion
{
	int begin;	// First character code in the region.
	int length;	// Number of characters in the region.
};


// A fixed-size bitmap font that stores glyph images
// for a region of character codes.
class Font
{
public:
	using sptr = cmg::shared_ptr<Font>;

public:
	~Font();

	// Accessors
	const Glyph* GetGlyph(char c) const;
	const Glyph* GetGlyph(int charCode) const;
	bool IsCharDefined(int charCode) const;
	inline int GetSize() const { return m_size; }
	inline const String& GetFamilyName() const { return m_familyName; }
	inline const String& GetStyleName() const { return m_styleName; }
	inline const Texture* GetGlyphTexture()	const { return m_glyphAtlasTexture; }
	inline Texture* GetGlyphTexture() { return m_glyphAtlasTexture; }

	// FreeType library
	static bool InitFreeTypeLibrary();
	static void QuitFreeTypeLibrary();
	static Error LoadFont(Font*& outFont, const Path& path, int size,
		int charRegionBegin, int charRegionEnd);

private:
	Font(void* ftFace, int size, int charRegionBegin, int charRegionEnd);

	// Prevent copying
	Font(const Font& other) {}
	void operator=(const Font& other) {}

	String m_familyName;
	String m_styleName;
	int m_size;
	Glyph* m_glyphs;
	Texture* m_glyphAtlasTexture;
	CharacterRegion m_charRegion;
};



#endif // _CMG_GRAPHICS_FONT_H_