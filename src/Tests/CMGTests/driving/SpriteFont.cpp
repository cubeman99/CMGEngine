#include "SpriteFont.h"


SpriteFont::SpriteFont() :
	m_texture(nullptr),
	m_charsPerRow(0),
	m_charWidth(0),
	m_charHeight(0),
	m_charSpacing(0)
{
}

SpriteFont::~SpriteFont()
{
	delete m_texture;
}


SpriteFont* SpriteFont::LoadFont(const Path& path, int charsPerRow, int charWidth, int charHeight, int charSpacing)
{
	Texture* texture = Texture::LoadTexture(path.GetPath());
	if (texture == nullptr)
		return nullptr;

	SpriteFont* spriteFont = new SpriteFont();
	spriteFont->m_texture = texture;
	spriteFont->m_charsPerRow = charsPerRow;
	spriteFont->m_charWidth = charWidth;
	spriteFont->m_charHeight = charHeight;
	spriteFont->m_charSpacing = charSpacing;
	return spriteFont;
}
/*
void SpriteFont::DrawString(const char* text, const Vector2f& position, const Vector4f& color, float scale)
{
	Vector2f charSize(m_charWidth  / (float) m_imageWidth,
					  m_charHeight / (float) m_imageHeight);
		
	glBindTexture(GL_TEXTURE_2D, m_glTextureId);
	glBegin(GL_QUADS);
	glColor4fv(&color.x);
	
	Vector2f cursor = position;
	Vector2f screenCharSize = Vector2f(m_charWidth * scale, m_charHeight * scale);
	
	Vector2f texCoord;
	int row, col, x, y;

	for (const char* charPtr = text; *charPtr != NULL; charPtr++)
	{
		unsigned char c = (unsigned char) *charPtr;

		if (c == '\n')
		{
			cursor.x = position.x;
			cursor.y += m_charHeight * scale;
		}
		else
		{
			col = c % m_charsPerRow;
			row = c / m_charsPerRow;
			x   = col * (m_charWidth  + m_charSpacing);
			y   = row * (m_charHeight + m_charSpacing);
			texCoord.x = x / (float) m_imageWidth;
			texCoord.y = y / (float) m_imageHeight;

			glTexCoord2f(texCoord.x, texCoord.y);
			glVertex2f(cursor.x, cursor.y);
			glTexCoord2f(texCoord.x + charSize.x, texCoord.y);
			glVertex2f(cursor.x + screenCharSize.x, cursor.y);
			glTexCoord2f(texCoord.x + charSize.x, texCoord.y + charSize.y);
			glVertex2f(cursor.x + screenCharSize.x, cursor.y + screenCharSize.y);
			glTexCoord2f(texCoord.x, texCoord.y + charSize.y);
			glVertex2f(cursor.x, cursor.y + screenCharSize.y);

			cursor.x += m_charWidth * scale;
		}
	}

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}
*/

