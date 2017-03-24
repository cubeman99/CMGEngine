#include "cmgGraphics2D.h"


namespace
{
	void gl_Transform(Matrix4f& transform)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(transform.GetTranspose().data());
	}

	void gl_Vertex(float x, float y, float u, float v)
	{
		glTexCoord2f(u, v);
		glVertex2f(x, y);
	}

	void gl_Vertex(const Vector2f& pos, float u, float v)
	{
		glTexCoord2f(u, v);
		glVertex2fv(pos.data());
	}

	void gl_Color(const Color& col)
	{
		Vector4f v = col.ToVector4f();
		glColor4f(v.x, v.y, v.z, v.w);
	}

	void gl_Rect(const Rect2f& r)
	{
		glVertex2f(r.position.x, r.position.y);
		glVertex2f(r.position.x + r.size.x, r.position.y);
		glVertex2f(r.position.x + r.size.x, r.position.y + r.size.y);
		glVertex2f(r.position.x, r.position.y + r.size.y);
	}
	
	void gl_TexturedRect(const Rect2f& r)
	{
		glTexCoord2f(0, 0);
		glVertex2f(r.position.x, r.position.y);
		glTexCoord2f(1, 0);
		glVertex2f(r.position.x + r.size.x, r.position.y);
		glTexCoord2f(1, 1);
		glVertex2f(r.position.x + r.size.x, r.position.y + r.size.y);
		glTexCoord2f(0, 1);
		glVertex2f(r.position.x, r.position.y + r.size.y);
	}
};


Shader* Graphics2D::s_shader = nullptr;



Graphics2D::Graphics2D(Window* window) :
	m_transformation(Matrix4f::IDENTITY),
	m_window(window)
{
	
	if (s_shader == nullptr)
	{
		String vertexSource =
			"#version 330 core\n"
			"in vec3 a_vertPos;\n"
			"in vec2 a_vertTexCoord;\n"
			"out vec2 v_texCoord;\n"
			"uniform mat4 u_mvp;\n"
			"void main()\n"
			"{\n"
				"gl_Position = u_mvp * vec4(a_vertPos, 1);\n"
				"v_texCoord = a_vertTexCoord;\n"
			"}";
		String fragmentSource =
			"#version 330 core\n"
			"in vec2 v_texCoord;\n"
			"out vec4 o_color;\n"
			"uniform sampler2D s_texture;\n"
			"uniform vec4 u_color;\n"
			"void main()\n"
			"{\n"
				"o_color = texture2D(s_texture, v_texCoord) * u_color;\n"
			"}";
		s_shader = new Shader();
		s_shader->AddStage(ShaderType::k_vertex_shader, vertexSource, "");
		s_shader->AddStage(ShaderType::k_fragment_shader, fragmentSource, "");
		s_shader->CompileAndLink();
	}

	Matrix4f projection = Matrix4f::CreateOrthographic(
		0, (float) window->GetWidth(), (float) window->GetHeight(), 0, -1, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(projection.GetTranspose().data());
}

void Graphics2D::SetTransformation(const Matrix4f& transformation)
{
	m_transformation = transformation;
}

void Graphics2D::Clear(const Color& clearColor)
{
	Vector4f c = clearColor.ToVector4f();
	glClearColor(c.x, c.y, c.z, c.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics2D::DrawTexture(Texture* texture, float x, float y, const Color& color)
{
	float w = (float) texture->GetWidth();
	float h = (float) texture->GetHeight();

	gl_Transform(m_transformation);
	glBindTexture(GL_TEXTURE_2D, texture->GetGLTextureID());
	glBegin(GL_QUADS);
		gl_Color(color);
		gl_Vertex(x, y, 0, 0);
		gl_Vertex(x + w, y, 1, 0);
		gl_Vertex(x + w, y + h, 1, 1);
		gl_Vertex(x, y + h, 0, 1);
	glEnd();
}

void Graphics2D::DrawTexture(Texture* texture, const Vector2f& position, const Color& color)
{
	float w = (float) texture->GetWidth();
	float h = (float) texture->GetHeight();
	
	gl_Transform(m_transformation);
	glBindTexture(GL_TEXTURE_2D, texture->GetGLTextureID());
	glBegin(GL_QUADS);
		gl_Color(color);
		gl_TexturedRect(Rect2f(position, Vector2f(w, h)));
	glEnd();
}

void Graphics2D::DrawTexture(Texture* texture, const Rect2f& destination, const Color& color)
{
	gl_Transform(m_transformation);
	glBindTexture(GL_TEXTURE_2D, texture->GetGLTextureID());
	glBegin(GL_QUADS);
		gl_Color(color);
		gl_TexturedRect(destination);
	glEnd();
}

void Graphics2D::DrawTexture(Texture* texture, const Rect2f& source, const Rect2f& destination, const Color& color)
{
	Rect2f src = source;
	src.Scale(1.0f / (float) texture->GetWidth(), 1.0f / (float) texture->GetHeight());

	gl_Transform(m_transformation);
	glBindTexture(GL_TEXTURE_2D, texture->GetGLTextureID());
	glBegin(GL_QUADS);
		gl_Color(color);
		glTexCoord2f(src.position.x, src.position.y);
		glVertex2f(destination.position.x, destination.position.y);
		glTexCoord2f(src.position.x + src.size.x, src.position.y);
		glVertex2f(destination.position.x + destination.size.x, destination.position.y);
		glTexCoord2f(src.position.x + src.size.x, src.position.y + src.size.y);
		glVertex2f(destination.position.x + destination.size.x, destination.position.y + destination.size.y);
		glTexCoord2f(src.position.x, src.position.y + src.size.y);
		glVertex2f(destination.position.x, destination.position.y + destination.size.y);
	glEnd();
}


void Graphics2D::DrawRect(float x, float y, float width, float height, const Color& color)
{
	gl_Transform(m_transformation);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_LINE_LOOP);
		gl_Color(color);
		gl_Vertex(x, y, 0, 0);
		gl_Vertex(x + width, y, 1, 0);
		gl_Vertex(x + width, y + height, 1, 1);
		gl_Vertex(x, y + height, 0, 1);
	glEnd();
}

void Graphics2D::DrawRect(const Rect2f& rect, const Color& color)
{
	gl_Transform(m_transformation);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_LINE_LOOP);
		gl_Color(color);
		gl_Rect(rect);
	glEnd();
}

void Graphics2D::FillRect(float x, float y, float width, float height, const Color& color)
{
	gl_Transform(m_transformation);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
		gl_Color(color);
		gl_Vertex(x, y, 0, 0);
		gl_Vertex(x + width, y, 1, 0);
		gl_Vertex(x + width, y + height, 1, 1);
		gl_Vertex(x, y + height, 0, 1);
	glEnd();
}

void Graphics2D::FillRect(const Rect2f& rect, const Color& color)
{
	gl_Transform(m_transformation);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
		gl_Color(color);
		gl_Rect(rect);
	glEnd();
}


void Graphics2D::DrawLine(const Vector2f& a, const Vector2f& b, const Color& color)
{
	gl_Transform(m_transformation);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_LINES);
		gl_Color(color);
		glVertex2fv(a.data());
		glVertex2fv(b.data());
	glEnd();
}

void Graphics2D::DrawCircle(const Vector2f& center, float radius, const Color& color)
{
	gl_Transform(m_transformation);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_LINE_LOOP);
		gl_Color(color);
		int points = 20;
		for (int i = 0; i < points; i++)
		{
			float angle = (i / (float) points) * Math::TWO_PI;
			Vector2f v(Math::Cos(angle) * radius, Math::Sin(angle) * radius);
			v += center;
			glVertex2fv(v.data());
		}
	glEnd();
}

void Graphics2D::FillCircle(const Vector2f& center, float radius, const Color& color)
{
	gl_Transform(m_transformation);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_TRIANGLE_FAN);
		gl_Color(color);
		int points = 20;
		for (int i = 0; i < points; i++)
		{
			float angle = (i / (float) points) * Math::TWO_PI;
			Vector2f v(Math::Cos(angle) * radius, Math::Sin(angle) * radius);
			v += center;
			glVertex2fv(v.data());
		}
	glEnd();
}