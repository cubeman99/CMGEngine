#ifndef _CMG_GRAPHICS_2D_H_
#define _CMG_GRAPHICS_2D_H_

#include <cmgGraphics/cmgVertexData.h>
#include <cmgGraphics/cmgTexture.h>
#include <cmgGraphics/cmgShader.h>
#include <cmgGraphics/cmgWindow.h>
#include <cmgMath/cmg_math.h>


class Graphics2D
{
public:
	Graphics2D(Window* window);

	void SetTransformation(const Matrix4f& transformation);
	
	void Clear(const Color& clearColor);

	void DrawTexture(Texture* texture, float x, float y, const Color& color = Color::WHITE);
	void DrawTexture(Texture* texture, const Vector2f& position, const Color& color = Color::WHITE);
	void DrawTexture(Texture* texture, const Rect2f& destination, const Color& color = Color::WHITE);
	void DrawTexture(Texture* texture, const Rect2f& source, const Rect2f& destination, const Color& color = Color::WHITE);

	void DrawRect(float x, float y, float width, float height, const Color& color);
	void DrawRect(const Rect2f& rect, const Color& color);
	void FillRect(float x, float y, float width, float height, const Color& color);
	void FillRect(const Rect2f& rect, const Color& color);

	void DrawLine(const Vector2f& a, const Vector2f& b, const Color& color, float width = 1.0f);

	void DrawCircle(const Vector2f& center, float radius, const Color& color);
	void FillCircle(const Vector2f& center, float radius, const Color& color);

private:
	VertexBuffer m_vertexBuffer;

	static Shader* s_shader;

	Matrix4f m_transformation;
	Window* m_window;
};


#endif // _CMG_GRAPHICS_2D_H_