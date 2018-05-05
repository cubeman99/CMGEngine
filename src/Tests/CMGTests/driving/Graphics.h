#ifndef _DRIVE_GRAPHICS_H_
#define _DRIVE_GRAPHICS_H_

#include <cmgApplication/cmg_application.h>
#include <cmgGraphics/cmg_graphics.h>
#include <cmgMath/cmg_math.h>
#include "Graph.h"


struct Trail
{
	std::vector<VertexPosCol> m_points;
	Color m_color;

	Trail()
	{
	}

	Trail(const Color& color) :
		m_color(color)
	{
	}

	void Clear()
	{
		m_points.clear();
	}

	void AddPoint(const Vector3f& point, const Vector4f& color = Vector4f::ONE)
	{
		unsigned int maxPoints = 60 * 20;
		m_points.push_back(VertexPosCol(point, color));
		if (m_points.size() > maxPoints)
			m_points.erase(m_points.begin());
	}
	
	inline std::vector<VertexPosCol>::iterator points_begin() { return m_points.begin(); }
	inline std::vector<VertexPosCol>::iterator points_end() { return m_points.end(); }
	inline std::vector<VertexPosCol>::const_iterator points_begin() const { return m_points.begin(); }
	inline std::vector<VertexPosCol>::const_iterator points_end() const { return m_points.end(); }
	
	inline void SetColor(const Color& color) { m_color = color; }
	inline const Color& GetColor() const { return m_color; }
};


//struct TextAlign
//{
//	enum
//	{
//		k_center	= 0x0,
//		k_middle	= 0x0,
//		k_top		= 0x1,
//		k_bottom	= 0x2,
//		k_left		= 0x4,
//		k_right		= 0x8,
//
//		k_top_left		= k_top | k_left,
//		k_top_right		= k_top | k_right,
//		k_top_center	= k_top | k_center,
//		k_bottom_left	= k_bottom | k_left,
//		k_bottom_right	= k_bottom | k_right,
//		k_bottom_center	= k_bottom | k_center,
//		k_middle_left	= k_middle | k_left,
//		k_middle_right	= k_middle | k_right,
//		k_centered		= k_middle | k_center,
//	};
//};


class Graphics
{
public:
	Graphics();

	void Initialize(Window* window, SpriteFont* font);

	void Clear(const Color& color);
	
	void DrawLine(float x1, float y1, float x2, float y2, const Color& color, float lineWidth = 1.0f);
	void DrawLine(const Vector2f& from, const Vector2f& to, const Color& color, float lineWidth = 1.0f);
	void DrawRect(const Vector2f& pos, const Vector2f& size, const Color& color, float lineWidth = 1.0f);
	void DrawRect(float x, float y, float width, float height, const Color& color, float lineWidth = 1.0f);
	void FillRect(const Vector2f& pos, const Vector2f& size, const Color& color);
	void FillRect(float x, float y, float width, float height, const Color& color);
	void DrawCircle(const Vector2f& pos, float radius, const Color& color, int numEdges = 20, float lineWidth = 1.0f);
	void FillCircle(const Vector2f& pos, float radius, const Color& color, int numEdges = 20);

	void DrawTrail(const Trail& trail);
	void DrawGraph(const Graph& graph, const Vector2f& position, const Vector2f& size);

	void DrawBar(const Vector2f& pos, const Vector2f& size, float fillAmount, const Color& fillColor, const Color& outlineColor);

	void DrawPoint(const Vector3f& point, const Color& color, float size);
	void DrawArrow(const Vector3f& point, const Vector3f& size, const Color& color, float arrowRadius, float lineThickness = 2.0f);
	void DrawArrow(const Vector3f& point, const Vector3f& size, const Vector3f& perpendicularDir, const Color& color, float arrowRadius, float lineThickness = 2.0f);
	void DrawString(const String& text, const Vector2f& position, const Color& color, TextAlign align = TextAlign::TOP_LEFT);
	void DrawString(const SpriteFont* font, const String& text, const Vector2f& position, const Color& color, TextAlign align = TextAlign::TOP_LEFT);
	
	Vector2f MeasureString(const String& text);
	Vector2f MeasureString(const SpriteFont* font, const String& text);

private:
	void gl_Color(const Color& color);
	void gl_Vertex(const Vector3f& vertex);
	void gl_Vertex(const Vector2f& vertex);
	void gl_Vertex(float x, float y);
	void gl_Vertex(float x, float y, float z);

	Vector2f m_windowSize;
	SpriteFont* m_font;
};


#endif // _DRIVE_GRAPHICS_H_