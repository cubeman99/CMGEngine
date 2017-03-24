#include "Graphics.h"



Graphics::Graphics() :
	m_font(nullptr)
{
}

void Graphics::Initialize(Window* window, SpriteFont* font)
{
	m_windowSize.x = (float) window->GetWidth();
	m_windowSize.y = (float) window->GetHeight();
	m_font = font;
}

void Graphics::Clear(const Color& color)
{
	Vector4f c = color.ToVector4f();
	glClearColor(c.x, c.y, c.z, c.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Graphics::DrawLine(float x1, float y1, float x2, float y2, const Color& color, float lineWidth)
{
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
	gl_Color(color);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
	glLineWidth(1.0f);
}

void Graphics::DrawLine(const Vector2f& from, const Vector2f& to, const Color& color, float lineWidth)
{
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
	gl_Color(color);
	glVertex2f(from.x, from.y);
	glVertex2f(to.x, to.y);
	glEnd();
	glLineWidth(1.0f);
}

void Graphics::DrawRect(const Vector2f& pos, const Vector2f& size, const Color& color, float lineWidth)
{
	glLineWidth(lineWidth);
	glBegin(GL_LINE_LOOP);
	gl_Color(color);
	glVertex2f(pos.x, pos.y);
	glVertex2f(pos.x + size.x, pos.y);
	glVertex2f(pos.x + size.x, pos.y + size.y);
	glVertex2f(pos.x, pos.y + size.y);
	glEnd();
	glLineWidth(1.0f);
}

void Graphics::DrawRect(float x, float y, float width, float height, const Color& color, float lineWidth)
{
	glLineWidth(lineWidth);
	glBegin(GL_LINE_LOOP);
	gl_Color(color);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
	glLineWidth(1.0f);
}

void Graphics::FillRect(const Vector2f& pos, const Vector2f& size, const Color& color)
{
	glBegin(GL_QUADS);
	gl_Color(color);
	glVertex2f(pos.x, pos.y);
	glVertex2f(pos.x + size.x, pos.y);
	glVertex2f(pos.x + size.x, pos.y + size.y);
	glVertex2f(pos.x, pos.y + size.y);
	glEnd();
}

void Graphics::FillRect(float x, float y, float width, float height, const Color& color)
{
	glBegin(GL_QUADS);
	gl_Color(color);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
}

void Graphics::DrawCircle(const Vector2f& pos, float radius, const Color& color, int numEdges, float lineWidth)
{
	glLineWidth(lineWidth);
	glBegin(GL_LINE_LOOP);
	gl_Color(color);
	for (int i = 0; i < numEdges; i++)
	{
		float angle = (i / (float) numEdges) * Math::TWO_PI;
		Vector2f v = pos + Vector2f(Math::Cos(angle) * radius, Math::Sin(angle) * radius);
		glVertex2fv(v.data());
	}
	glEnd();
	glLineWidth(1.0f);
}

void Graphics::FillCircle(const Vector2f& pos, float radius, const Color& color, int numEdges)
{
	glBegin(GL_TRIANGLE_FAN);
	gl_Color(color);
	for (int i = 0; i < numEdges; i++)
	{
		float angle = (i / (float) numEdges) * Math::TWO_PI;
		Vector2f v = pos + Vector2f(Math::Cos(angle) * radius, Math::Sin(angle) * radius);
		glVertex2fv(v.data());
	}
	glEnd();
}

void Graphics::DrawPoint(const Vector3f& point, const Color& color, float size)
{
	glPointSize(size);
	glPushAttrib(GL_POINT_SIZE);
	glBegin(GL_POINTS);
		gl_Color(color);
		gl_Vertex(point);
	glEnd();
	glPointSize(1.0f);
}

void Graphics::DrawTrail(const Trail& trail)
{
    glDepthMask(false);
	glBegin(GL_LINE_STRIP);
		gl_Color(trail.GetColor());
		for (auto it = trail.points_begin(); it != trail.points_end(); it++)
		{
			glColor4fv((it->color * trail.GetColor().ToVector4f()).data());
			gl_Vertex(it->position);
		}
	glEnd();
    glDepthMask(true);
}

void Graphics::DrawGraph(const Graph& graph, const Vector2f& position, const Vector2f& size)
{
	float titleHeight = m_font->m_charHeight + 6.0f;

	Vector2f boxPos = position;
	Vector2f boxSize = size;
	boxPos.y += titleHeight;
	boxSize.y -= titleHeight;

	// Draw background.
	glBegin(GL_QUADS);
		gl_Color(graph.m_backgroundColor);
		gl_Vertex(position.x, position.y);
		gl_Vertex(position.x + size.x, position.y);
		gl_Vertex(position.x + size.x, position.y + size.y);
		gl_Vertex(position.x, position.y + size.y);
	glEnd();

	// Draw axes.
	Vector2f origin;
	graph.GetPointInWindow(Vector2f::ZERO, origin);
	glBegin(GL_LINES);
		gl_Color(graph.m_axisColor);
		if (origin.y >= 0.0f && origin.y <= 1.0f)
		{
			gl_Vertex(boxPos.x, boxPos.y + (boxSize.y * origin.y));
			gl_Vertex(boxPos.x + boxSize.x, boxPos.y + (boxSize.y * origin.y));
		}
		if (origin.x >= 0.0f && origin.x <= 1.0f)
		{
			gl_Vertex(boxPos.x + (boxSize.x * origin.x), boxPos.y);
			gl_Vertex(boxPos.x + (boxSize.x * origin.x), boxPos.y + boxSize.y);
		}
	glEnd();

	// Draw data line.
	glBegin(GL_LINE_STRIP);
		gl_Color(graph.m_lineColor);
		for (unsigned int i = 0; i < graph.m_points.size(); i++)
		{
			Vector2f point;
			if (graph.GetPointInWindow(graph.m_points[i], point))
				gl_Vertex(boxPos + (boxSize * point));
		}
	glEnd();

	// Draw data points.
	//glPointboxSize(2.0f);
	//glBegin(GL_POINTS);
	//	gl_Color(graph.m_lineColor);
	//	for (unsigned int i = 0; i < graph.m_points.size(); i++)
	//	{
	//		Vector2f point;
	//		if (graph.GetPointInWindow(graph.m_points[i], point))
	//			gl_Vertex(boxPos + (boxSize * point));
	//	}
	//glEnd();
	//glPointboxSize(1.0f);

	// Draw marker.
	Vector2f markerPos;
	markerPos.x = graph.m_markerPosition;
	markerPos.y = 0.0f;

	Vector2f m0;
	graph.GetPointInWindow(markerPos, m0);

	if (m0.x >= 0.0f && m0.x <= 1.0f)
	{
		markerPos.y = graph.GetInterpolatedValue(graph.m_markerPosition);
		Vector2f m1;
		Vector2f m2;
		//graph.GetPointInWindow(markerPos, m1);
		//graph.GetPointInWindow(Vector2f(markerPos.x, 0.0f), m2);
		m1.x = m0.x;
		m2.x = m0.x;
		m1.y = 0.0f;//Math::Clamp(m1.y, 0.0f, 1.0f);
		m2.y = 1.0f;//Math::Clamp(m2.y, 0.0f, 1.0f);

		glBegin(GL_LINES);
			gl_Color(graph.m_markerColor * 0.8f);
			gl_Vertex(boxPos + (boxSize * m1));
			gl_Vertex(boxPos + (boxSize * m2));
		glEnd();
		
		graph.GetPointInWindow(markerPos, m0);
		if (m0.y >= 0.0f && m0.y <= 1.0f)
		{
			glPointSize(4.0f);
			glBegin(GL_POINTS);
				gl_Color(graph.m_markerColor);
				gl_Vertex(boxPos + (boxSize * m0));
			glEnd();
		}
	}

	// Draw box outline.
	glBegin(GL_LINE_LOOP);
		gl_Color(graph.m_outlineColor);
		gl_Vertex(boxPos.x, boxPos.y);
		gl_Vertex(boxPos.x + boxSize.x, boxPos.y);
		gl_Vertex(boxPos.x + boxSize.x, boxPos.y + boxSize.y);
		gl_Vertex(boxPos.x, boxPos.y + boxSize.y);
	glEnd();

	// Draw title.
	float titleWidth = (float) graph.m_title.length() * (float) m_font->m_charWidth;
	DrawString(graph.m_title, position +
		Vector2f((size.x - titleWidth) * 0.5f, 3), graph.m_textColor);

	// Draw outline.
	glBegin(GL_LINE_LOOP);
		gl_Color(graph.m_outlineColor);
		gl_Vertex(position.x, position.y);
		gl_Vertex(position.x + size.x, position.y);
		gl_Vertex(position.x + size.x, position.y + size.y);
		gl_Vertex(position.x, position.y + size.y);
	glEnd();
	
	glPointSize(1.0f);
}

void Graphics::DrawBar(const Vector2f& pos, const Vector2f& size, float fillAmount, const Color& fillColor, const Color& outlineColor)
{
	FillRect(Vector2f(pos.x, pos.y - size.y * (1.0f - fillAmount)),
			 Vector2f(size.x, size.y * fillAmount), fillColor);
	DrawRect(pos, size, outlineColor);
}


void Graphics::DrawArrow(const Vector3f& point, const Vector3f& size, const Color& color, float arrowRadius, float lineThickness)
{
	DrawArrow(point, size, Vector3f::UP, color, arrowRadius, lineThickness);
}

void Graphics::DrawArrow(const Vector3f& point, const Vector3f& size, const Vector3f& perpendicularDir, const Color& color, float arrowRadius, float lineThickness)
{
	glLineWidth(lineThickness);

	Vector3f up = perpendicularDir;
	up.Normalize();
	Vector3f right = size.Cross(up);
	right.Normalize();
	Vector3f back = -size;
	back.Normalize();
	Vector3f headTip = point + size;
	Vector3f headEnd1 = headTip + Vector3f::Normalize(back + right) * arrowRadius;
	Vector3f headEnd2 = headTip + Vector3f::Normalize(back - right) * arrowRadius;

	glBegin(GL_LINES);
		glColor4ubv(color.data());
		glVertex3fv(point.data());
		glVertex3fv(headTip.data());
		glVertex3fv(headTip.data());
		glVertex3fv(headEnd1.data());
		glVertex3fv(headTip.data());
		glVertex3fv(headEnd2.data());
	glEnd();

	glLineWidth(1.0f);
}

void Graphics::DrawString(const String& text, const Vector2f& position, const Color& color, int align)
{
	DrawString(m_font, text, position, color, align);
}

void Graphics::DrawString(const SpriteFont* font, const String& text, const Vector2f& position, const Color& color, int align)
{
	float scale = 1.0f;

	Texture* texture = font->GetTexture();
	Vector2f texSize((float) texture->GetWidth(), (float) texture->GetHeight());
	Vector2f charSize = Vector2f((float) font->m_charWidth , (float) font->m_charHeight) / texSize;
	Vector2f cursor = position;
	Vector2f screenCharSize = Vector2f(font->m_charWidth * scale, font->m_charHeight * scale);
	Vector2f texCoord;
	int row, col, x, y;

	// Change cursor position based on alignment.
	Vector2f stringSize = MeasureString(font, text);
	if (align & TextAlign::k_right)
		cursor.x -= stringSize.x;
	if (!(align & TextAlign::k_left))
		cursor.x -= (int) (stringSize.x * 0.5f);
	if (align & TextAlign::k_bottom)
		cursor.y -= stringSize.y;
	else if (!(align & TextAlign::k_top))
		cursor.y -= (int) (stringSize.y * 0.5f);

	glBindTexture(GL_TEXTURE_2D, font->GetTexture()->GetGLTextureID());
	glBegin(GL_QUADS);
	glColor4ubv(color.data());

	for (const char* charPtr = text.c_str(); *charPtr != '\0'; charPtr++)
	{
		unsigned char c = (unsigned char) *charPtr;

		if (c == '\n')
		{
			cursor.x = position.x;
			cursor.y += font->m_charHeight * scale;
		}
		else
		{
			col = c % font->m_charsPerRow;
			row = c / font->m_charsPerRow;
			x   = col * (font->m_charWidth  + font->m_charSpacing);
			y   = row * (font->m_charHeight + font->m_charSpacing);
			texCoord.x = x / (float) font->GetTexture()->GetWidth();
			texCoord.y = y / (float) font->GetTexture()->GetHeight();

			glTexCoord2f(texCoord.x, texCoord.y);
			glVertex2f(cursor.x, cursor.y);
			glTexCoord2f(texCoord.x + charSize.x, texCoord.y);
			glVertex2f(cursor.x + screenCharSize.x, cursor.y);
			glTexCoord2f(texCoord.x + charSize.x, texCoord.y + charSize.y);
			glVertex2f(cursor.x + screenCharSize.x, cursor.y + screenCharSize.y);
			glTexCoord2f(texCoord.x, texCoord.y + charSize.y);
			glVertex2f(cursor.x, cursor.y + screenCharSize.y);

			cursor.x += font->m_charWidth * scale;
		}
	}

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}


Vector2f Graphics::MeasureString(const String& text)
{
	return MeasureString(m_font, text);
}

Vector2f Graphics::MeasureString(const SpriteFont* font, const String& text)
{
	Vector2f cursor(0, 0);
	Vector2f size(0, 0);

	for (const char* charPtr = text.c_str(); *charPtr != '\0'; charPtr++)
	{
		unsigned char c = (unsigned char) *charPtr;

		if (c == '\n')
		{
			cursor.x = 0.0f;
			cursor.y += font->m_charHeight;
		}
		else
		{
			cursor.x += font->m_charWidth;
		}

		size.x = Math::Max(size.x, cursor.x);
		size.y = Math::Max(size.y, cursor.y + font->m_charHeight);
	}

	return size;
}

void Graphics::gl_Color(const Color& color)
{
	glColor4ubv(color.data());
}

void Graphics::gl_Vertex(const Vector3f& vertex)
{
	glVertex3fv(vertex.data());
}

void Graphics::gl_Vertex(const Vector2f& vertex)
{
	glVertex2fv(vertex.data());
}

void Graphics::gl_Vertex(float x, float y)
{
	glVertex2f(x, y);
}

void Graphics::gl_Vertex(float x, float y, float z)
{
	glVertex3f(x, y, z);
}
