#include "cmgSpriteFont.h"

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

SpriteFont::SpriteFont()
	: m_texture(nullptr)
	, m_charsPerRow(0)
	, m_charWidth(0)
	, m_charHeight(0)
	, m_charSpacing(0)
{
}

SpriteFont::~SpriteFont()
{
	delete m_texture;
}


//-----------------------------------------------------------------------------
// Static Methods
//-----------------------------------------------------------------------------

SpriteFont* SpriteFont::LoadSpriteFont(const Path& path, int charsPerRow,
	int charWidth, int charHeight, int charSpacing)
{
	// Load the sprite font image
	Texture* texture = nullptr;
	Error error = Texture::LoadTexture(texture, path);
	if (error.Failed())
		return nullptr;

	// Construct the sprite font object
	SpriteFont* spriteFont = new SpriteFont();
	spriteFont->m_texture = texture;
	spriteFont->m_charsPerRow = charsPerRow;
	spriteFont->m_charWidth = charWidth;
	spriteFont->m_charHeight = charHeight;
	spriteFont->m_charSpacing = charSpacing;
	return spriteFont;
}

SpriteFont* SpriteFont::LoadBuiltInFont(BuiltInFonts builtInFont)
{
	if (builtInFont == BuiltInFonts::FONT_CONSOLE)
	{
		// Convert the bitmap data into RGBA pixels
		const unsigned int width = FontConsoleBitmap::WIDTH;
		const unsigned int height = FontConsoleBitmap::HEIGHT;
		const unsigned int size = width * height;
		Color pixels[size];
		unsigned int alpha;
		for (unsigned int i = 0; i < size; i++)
		{
			alpha = (FontConsoleBitmap::BITMAP_DATA[i] > 0 ? 255 : 0);
			pixels[i] = Color(255, 255, 255, alpha);
		}

		// Load the pixels into a texture
		TextureParams params;
		params.SetFiltering(TextureFilter::NEAREST);
		params.SetWrap(TextureWrap::CLAMP_TO_BORDER);
		Texture* texture = new Texture(params);
		texture->WritePixels2D(width, height, pixels);

		// Construct the sprite font object
		SpriteFont* spriteFont = new SpriteFont();
		spriteFont->m_texture = texture;
		spriteFont->m_charsPerRow = FontConsoleBitmap::CHARS_PER_ROW;
		spriteFont->m_charWidth = FontConsoleBitmap::CHAR_WIDTH;
		spriteFont->m_charHeight = FontConsoleBitmap::CHAR_HEIGHT;
		spriteFont->m_charSpacing = FontConsoleBitmap::CHAR_SPACING;
		return spriteFont;
	}
	else
	{
		return nullptr;
	}
}
