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
	
	void SetWindowOrthoProjection();
	void SetProjection(const Matrix4f& projection);
	void SetTransformation(const Matrix4f& transformation);
	void SetRenderTarget(RenderTarget* renderTarget);

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

	Rect2f MeasureString(const Font* font, const String& string);
	void DrawString(Font* font, const String& string,
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


private:
	void ActivateRenderTarget();

	VertexBuffer m_vertexBuffer;

	static Shader* s_shader;

	RenderTarget* m_renderTarget;
	Matrix4f m_transformation;
	Window* m_window;
};


#endif // _CMG_GRAPHICS_2D_H_