#include <ft2build.h>
#include FT_FREETYPE_H
#include "cmgFont.h"
#include <cmgGraphics/utilities/cmgShelfBinPacker.h>
#include <cmgOpenGLIncludes.h>


//-----------------------------------------------------------------------------
// Glyph
//-----------------------------------------------------------------------------

// Initialize the glyph metrics.
void Glyph::Init(void* ftGlyphPtr)
{
	FT_GlyphSlot ftGlyph = static_cast<FT_GlyphSlot>(ftGlyphPtr);
	m_sourceX = 0;
	m_sourceY = 0;
	m_minX = ftGlyph->bitmap_left;
	m_minY = -ftGlyph->bitmap_top;
	m_width = ftGlyph->bitmap.width;
	m_height = ftGlyph->bitmap.rows;
	m_advance = ftGlyph->advance.x / 64;
}



//-----------------------------------------------------------------------------
// FreeType library
//-----------------------------------------------------------------------------

static FT_Library g_freeTypeLibrary = nullptr;


// Initialize the Free Type Library, returning whether it was successful.
bool Font::InitFreeTypeLibrary()
{
	if (g_freeTypeLibrary == nullptr)
	{
		FT_Error ftError = FT_Init_FreeType(&g_freeTypeLibrary);

		if (ftError != FT_Err_Ok)
		{
			std::cerr << "Error: failure to initialize FreeType library" << std::endl;
			return false;
		}
	}
	return true;
}

// Done using the Free Type Library.
void Font::QuitFreeTypeLibrary()
{
	if (g_freeTypeLibrary == nullptr)
		FT_Done_FreeType(g_freeTypeLibrary);
}

// Load a font from file, using the given size and character region.
Error Font::LoadFont(Font*& outFont, const Path& path, int32 size,
	uint32 charRegionBegin, uint32 charRegionEnd)
{
	InitFreeTypeLibrary();
	CMG_ASSERT(g_freeTypeLibrary != nullptr && charRegionEnd > charRegionBegin);
	FT_Error ftError;
	FT_Face ftFace;

	// Load the font face
	ftError = FT_New_Face(g_freeTypeLibrary, path.c_str(), 0, &ftFace);
	if (ftError == FT_Err_Unknown_File_Format)
	{
		std::cerr << "Error: unsupported font file format for " << path << std::endl;
		return CMG_ERROR_FAILURE;
	}
	else if (ftError != FT_Err_Ok)
	{
		std::cerr << "Error: failure to load font file " << path << std::endl;
		return CMG_ERROR_FAILURE;
	}

	// Set the font size
	ftError = FT_Set_Char_Size(ftFace, 0, size * 64, 96, 96);
	if (ftError != FT_Err_Ok)
	{
		std::cerr << "Error: the font " << path << " cannot support a size of " << size << std::endl;
		FT_Done_Face(ftFace);
		return CMG_ERROR_FAILURE;
	}

	// Create the font object.
	outFont = new Font(ftFace, size, charRegionBegin, charRegionEnd);
	return CMG_ERROR_SUCCESS;
}

Error Font::LoadSpriteFont(Font *& outFont, const Path & path,
	uint32 charsPerRow, uint32 charWidth, uint32 charHeight, uint32 charSpacing)
{
	// Load the sprite font image
	Texture* texture = nullptr;
	Error error = Texture::LoadTexture(texture, path);
	if (error.Failed())
		return error.Uncheck();

	// Create the font instance
	outFont = new Font(texture);

	// Create the mapping of glyphs using a grid pattern
	outFont->m_charRegion.begin = 0;
	outFont->m_charRegion.length = 256;
	outFont->m_glyphMap.clear();
	outFont->m_size = charWidth;
	for (uint32 i = 0; i < outFont->m_charRegion.length; i++)
	{
		uint32 charCode = outFont->m_charRegion.begin + i;
		uint32 x = i % charsPerRow;
		uint32 y = i / charsPerRow;
		Glyph glyph;
		glyph.m_sourceX = x * (charWidth + charSpacing);
		glyph.m_sourceY = y * (charHeight + charSpacing);
		glyph.m_width = charWidth;
		glyph.m_height = charHeight;
		glyph.m_advance = charWidth;
		glyph.m_minX = 0;
		glyph.m_minY = -((int32) charHeight);
		outFont->m_glyphMap[charCode] = glyph;
	}

	return CMG_ERROR_SUCCESS;
}

Error Font::LoadBuiltInFont(Font *& outFont, BuiltInFonts builtInFont)
{
	if (builtInFont == BuiltInFonts::FONT_CONSOLE)
	{
		// Convert the bitmap data into RGBA pixels
		const uint32 width = FontConsoleBitmap::WIDTH;
		const uint32 height = FontConsoleBitmap::HEIGHT;
		const uint32 size = width * height;
		Color pixels[size];
		uint32 alpha;
		for (uint32 i = 0; i < size; i++)
		{
			alpha = (FontConsoleBitmap::BITMAP_DATA[i] > 0 ? 255 : 0);
			pixels[i] = Color(255, 255, 255, alpha);
		}

		// Load the pixels into a texture
		TextureParams params;
		params.SetFiltering(TextureFilter::k_nearest);
		params.SetWrap(TextureWrap::k_clamp_to_border);
		Texture* texture = new Texture(params);
		texture->WritePixels2D(width, height, pixels);

		// Construct the sprite font object
		outFont = new Font(texture);

		// Create the mapping of glyphs using a grid pattern
		outFont->m_charRegion.begin = 0;
		outFont->m_charRegion.length = 256;
		outFont->m_size = FontConsoleBitmap::CHAR_WIDTH;
		for (uint32 i = 0; i < outFont->m_charRegion.length; i++)
		{
			uint32 charCode = outFont->m_charRegion.begin + i;
			uint32 x = i % FontConsoleBitmap::CHARS_PER_ROW;
			uint32 y = i / FontConsoleBitmap::CHARS_PER_ROW;
			Glyph glyph;
			glyph.m_sourceX = x * (FontConsoleBitmap::CHAR_WIDTH +
				FontConsoleBitmap::CHAR_SPACING);
			glyph.m_sourceY = y * (FontConsoleBitmap::CHAR_HEIGHT +
				FontConsoleBitmap::CHAR_SPACING);
			glyph.m_width = FontConsoleBitmap::CHAR_WIDTH;
			glyph.m_height = FontConsoleBitmap::CHAR_HEIGHT;
			glyph.m_advance = FontConsoleBitmap::CHAR_WIDTH;
			glyph.m_minX = 0;
			glyph.m_minY = -((int32) FontConsoleBitmap::CHAR_HEIGHT);
			outFont->m_glyphMap[charCode] = glyph;
		}
	}
	else
	{
		return CMG_ERROR_FAILURE;
	}

	return CMG_ERROR_SUCCESS;
}



//-----------------------------------------------------------------------------
// Font constructor & destructor
//-----------------------------------------------------------------------------

Font::Font(Texture* texture) :
	m_glyphAtlasTexture(texture)
{
}

Font::Font(void* ftFace, int32 size, uint32 charRegionBegin,
	uint32 charRegionEnd) :
	m_familyName(((FT_Face) ftFace)->family_name),
	m_styleName(((FT_Face) ftFace)->style_name),
	m_size(size)
{
	FT_Face pFace = static_cast<FT_Face>(ftFace);

	m_charRegion.begin = charRegionBegin;
	m_charRegion.length = charRegionEnd - charRegionBegin;

	Map<uint32, uint8*> glyphImageData;
	cmg::ShelfBinPacker rectPacker;
	FT_Error ftError;
	FT_UInt charIndex;
	uint32 charCode;

	// Create the glyphs
	for (uint32 i = 0; i < m_charRegion.length; i++)
	{
		charCode = m_charRegion.begin + i;

		// Check if the glyph exists in this font
		charIndex = FT_Get_Char_Index(pFace, static_cast<FT_ULong>(charCode));
		if (charIndex == 0)
			continue;

		// Load and render the glyph
		ftError = FT_Load_Char(pFace,
			static_cast<FT_ULong>(charCode), FT_LOAD_RENDER);
		if (ftError != FT_Err_Ok)
			continue;

		Glyph glyph;
		glyph.Init(pFace->glyph);

		// Copy the glyph's pixels into a luminance/alpha buffer
		if (pFace->glyph->bitmap.buffer != nullptr)
		{
			int32 glyphArea = glyph.m_width * glyph.m_height;
			uint8* pixels = new uint8[glyphArea * 4];
			for (int32 j = 0; j < glyphArea; j++)
			{
				pixels[(j * 4) + 0] = 255; // Luminence
				pixels[(j * 4) + 1] = 255; // Luminence
				pixels[(j * 4) + 2] = 255; // Luminence
				pixels[(j * 4) + 3] = pFace->glyph->bitmap.buffer[j]; // Alpha
			}
			glyphImageData[charCode] = pixels;
		}
		else
		{
			glyph.m_width = 0;
			glyph.m_height = 0;
		}

		m_glyphMap[charCode] = glyph;
	}

	// Pack the glyphs into a box
	for (auto it : m_glyphMap)
	{
		Glyph& glyph = m_glyphMap[it.first];
		if (glyph.HasImage())
		{
			rectPacker.Insert(&glyph.m_sourceX, &glyph.m_sourceY,
				glyph.m_width, glyph.m_height, 1);
		}
	}
	int32 texWidth = 512;
	int32 texHeight = 512;
	rectPacker.Pack(&texWidth, &texHeight);

	// Create the glyph atlas texture
	TextureParams params;
	params.SetWrap(TextureWrap::k_clamp_to_border);
	params.SetFiltering(TextureFilter::k_bilinear);
	m_glyphAtlasTexture = new Texture(params);
	glBindTexture(GL_TEXTURE_2D, m_glyphAtlasTexture->GetGLTextureID());
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2); // Pixel alignment defaults to 4, but we are using two 1-byte components (lum & alpha)
	m_glyphAtlasTexture->WritePixels2D(texWidth, texHeight,
		PixelTransferFormat::k_rgba, PixelType::k_unsigned_byte, nullptr);

	// Blit glyph images into the texture sheet
	for (auto it : m_glyphMap)
	{
		Glyph& glyph = it.second;
		if (glyph.HasImage())
		{
			glTexSubImage2D(GL_TEXTURE_2D,
				0,
				glyph.m_sourceX,
				glyph.m_sourceY,
				glyph.m_width,
				glyph.m_height,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				glyphImageData[it.first]);
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(pFace);
	for (auto it : glyphImageData)
		delete it.second;
}

Font::Font()
{
}

Font::~Font()
{
	delete m_glyphAtlasTexture;
	m_glyphAtlasTexture = nullptr;
}



//-----------------------------------------------------------------------------
// Accesors
//-----------------------------------------------------------------------------

// Return the glyph that represents the given character.
const Glyph* Font::GetGlyph(char c) const
{
	return GetGlyph(static_cast<uint32>(c));
}

// Return the glyph that represents the given character code.
const Glyph* Font::GetGlyph(uint32 charCode) const
{
	if (IsCharDefined(charCode))
		return (const Glyph*) &m_glyphMap.at(charCode);
	return nullptr;
}

// Is the given character code defined in the font's character regions?
bool Font::IsCharDefined(uint32 charCode) const
{
	return (charCode >= m_charRegion.begin &&
		charCode < m_charRegion.begin + m_charRegion.length &&
		m_glyphMap.find(charCode) != m_glyphMap.end());
}

