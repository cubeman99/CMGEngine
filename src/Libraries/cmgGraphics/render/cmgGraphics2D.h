#ifndef _CMG_GRAPHICS_2D_H_
#define _CMG_GRAPHICS_2D_H_

#include <cmgGraphics/cmgVertexData.h>
#include <cmgGraphics/cmgTexture.h>
#include <cmgGraphics/cmgFont.h>
#include <cmgGraphics/cmgShader.h>
#include <cmgGraphics/render/cmgRenderTarget.h>
#include <cmgGraphics/cmgWindow.h>
#include <cmgMath/cmg_math.h>


enum class TextAlign
{
	CENTER = 0x0,
	MIDDLE = 0x0,
	TOP = 0x1,
	BOTTOM = 0x2,
	LEFT = 0x4,
	RIGHT = 0x8,

	TOP_LEFT = TOP | LEFT,
	TOP_RIGHT = TOP | RIGHT,
	TOP_CENTER = TOP | CENTER,
	BOTTOM_LEFT = BOTTOM | LEFT,
	BOTTOM_RIGHT = BOTTOM | RIGHT,
	BOTTOM_CENTER = BOTTOM | CENTER,
	MIDDLE_LEFT = MIDDLE | LEFT,
	MIDDLE_RIGHT = MIDDLE | RIGHT,
	CENTERED = MIDDLE | CENTER,
};


class Graphics2D
{
public:
	Graphics2D(Window* window);
	virtual ~Graphics2D();

	void SetWindowOrthoProjection();
	void SetProjection(const Matrix4f& projection);
	void SetTransformation(const Matrix4f& transformation);
	void SetRenderTarget(RenderTarget* renderTarget);
	RenderTarget* GetRenderTarget();
	const Matrix4f& GetTransformation() const;

	void Clear(const Color& clearColor);

	//-----------------------------------------------------------------------------
	// Textures
	//-----------------------------------------------------------------------------

	void DrawTexture(Texture* texture, float x, float y,
		const Color& color = Color::WHITE);
	void DrawTexture(Texture* texture, const Vector2f& position,
		const Color& color = Color::WHITE);
	void DrawTexture(Texture* texture, const Rect2f& destination,
		const Color& color = Color::WHITE);
	void DrawTexture(Texture* texture, const Rect2f& source,
		const Rect2f& destination, const Color& color = Color::WHITE);

	//-----------------------------------------------------------------------------
	// Strings
	//-----------------------------------------------------------------------------

	template<class T_String>
	static Rect2f MeasureString(const Font* font, const T_String& string);
	//Rect2f MeasureString(const Font* font, const String& string);
	//void DrawString(Font* font, const String& string,
	//	const Vector2f& position, const Color& color = Color::WHITE,
	//	TextAlign align = TextAlign::TOP_LEFT);
	//void DrawString(Font* font, const std::u16string& string,
	//	const Vector2f& position, const Color& color = Color::WHITE,
	//	TextAlign align = TextAlign::TOP_LEFT);

	template<class T_String>
	void DrawString(Font* font, const T_String& string,
		const Vector2f& position, const Color& color = Color::WHITE,
		TextAlign align = TextAlign::TOP_LEFT);

	//-----------------------------------------------------------------------------
	// Rectangles
	//-----------------------------------------------------------------------------

	void DrawRect(float x, float y, float width, float height, const Color& color);
	void DrawRect(const Rect2f& rect, const Color& color);
	void FillRect(float x, float y, float width, float height, const Color& color);
	void FillRect(const Rect2f& rect, const Color& color);

	//-----------------------------------------------------------------------------
	// Lines
	//-----------------------------------------------------------------------------

	void DrawLine(const Vector2f& a, const Vector2f& b, const Color& color, float width = 1.0f);

	//-----------------------------------------------------------------------------
	// Circles
	//-----------------------------------------------------------------------------

	void DrawCircle(const Vector2f& center, float radius, const Color& color);
	void FillCircle(const Vector2f& center, float radius, const Color& color);


protected:
	void BeginDraw(Texture* texture);
	void EndDraw();
	void DrawGlyph(Texture* texture, const Glyph& glyph, const Vector2f& position, const Color& color);
	void ActivateRenderTarget();

	VertexBuffer m_vertexBuffer;

	static Shader* s_shader;

	RenderTarget* m_renderTarget;
	Matrix4f m_transformation;
	Window* m_window;
};

template <class T_String>
Rect2f Graphics2D::MeasureString(const Font* font, const T_String& string)
{
	const Glyph* glyph;
	Vector2i mins(0, 0);
	Vector2i maxs(0, 0);
	Vector2i penPosition(0, 0);

	for (auto c : string)
	{
		uint32 charCode = static_cast<uint32>(c);
		glyph = font->GetGlyph(charCode);
		if (glyph)
		{
			if (glyph->HasImage())
			{
				if (penPosition.x + glyph->GetMinX() < mins.x)
					mins.x = penPosition.x + glyph->GetMinX();
				if (penPosition.y + glyph->GetMinY() < mins.y)
					mins.y = penPosition.y + glyph->GetMinY();
				if (penPosition.x + glyph->GetMaxX() > maxs.x)
					maxs.x = penPosition.x + glyph->GetMaxX();
				if (penPosition.y + glyph->GetMaxY() > maxs.y)
					maxs.y = penPosition.y + glyph->GetMaxY();
			}
			penPosition.x += glyph->GetAdvance();
			maxs.x = Math::Max(penPosition.x, maxs.x);
		}
	}
	return Rect2f((float) mins.x, (float) mins.y,
		(float) (maxs.x - mins.x), (float) (maxs.y - mins.y));
}

template<class T_String>
void Graphics2D::DrawString(Font* font, const T_String& string,
	const Vector2f& position, const Color& color, TextAlign align)
{
	Texture* texture = font->GetGlyphTexture();
	const Glyph* glyph;

	Vector2f penPosition = position;
	float lineSpacing = font->GetSize() * 2.0f;

	if (align != TextAlign::BOTTOM_LEFT)
	{
		Rect2f bounds = MeasureString(font, string);
		if ((int) align & (int) TextAlign::RIGHT)
			penPosition.x -= bounds.GetRight();
		else if (!((int) align & (int) TextAlign::LEFT))
			penPosition.x -= bounds.GetRight() -
			(float) ((int) bounds.GetWidth() / 2);
		if ((int) align & (int) TextAlign::TOP)
			penPosition.y -= bounds.GetTop();
		else if (!((int) align & (int) TextAlign::BOTTOM))
			penPosition.y -= bounds.GetTop() +
			(float) ((int) bounds.GetHeight() / 2);
	}

	Vector2f penPositionStart = penPosition;
	BeginDraw(texture);

	// Draw each glyph in the string of characters
	for (auto c : string)
	{
		uint32 charCode = static_cast<uint32>(c);
		glyph = font->GetGlyph(charCode);

		if (charCode == (uint32) '\n')
		{
			penPosition.x = penPositionStart.x;
			penPosition.y += lineSpacing;
		}
		else if (glyph)
		{
			DrawGlyph(texture, *glyph, penPosition, color);
		}

		// Advance the pen position
		if (glyph)
			penPosition.x += glyph->GetAdvance();
	}

	EndDraw();
}

#endif // _CMG_GRAPHICS_2D_H_