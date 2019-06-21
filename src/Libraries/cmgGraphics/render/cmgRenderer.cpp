#include "cmgRenderer.h"
#include <cmgGraphics/cmgOpenGLIncludes.h>


namespace
{
	void EnableDisableGL(bool enabled, GLenum cap)
	{
		if (enabled)
			glEnable(cap);
		else
			glDisable(cap);
	}

	GLenum TranslateCompareFunc(CompareFunction::value_type compareFunc)
	{
		switch (compareFunc)
		{
		case CompareFunction::k_always_fail:	return GL_NEVER;
		case CompareFunction::k_less:			return GL_LESS;
		case CompareFunction::k_equal:			return GL_EQUAL;
		case CompareFunction::k_less_equal:		return GL_LEQUAL;
		case CompareFunction::k_greater:		return GL_GREATER;
		case CompareFunction::k_not_equal:		return GL_NOTEQUAL;
		case CompareFunction::k_greater_equal:	return GL_GEQUAL;
		case CompareFunction::k_always_pass:	return GL_ALWAYS;
		}
		return CompareFunction::k_less_equal;
	}

	GLenum TranslateBlendFunc(BlendFunc::value_type blendFunc)
	{
		switch (blendFunc)
		{
		case BlendFunc::k_zero:							return GL_ZERO;
		case BlendFunc::k_one:							return GL_ONE;
		case BlendFunc::k_source_color:					return GL_SRC_COLOR;
		case BlendFunc::k_one_minus_source_color:		return GL_ONE_MINUS_SRC_COLOR;
		case BlendFunc::k_destination_color:			return GL_DST_COLOR;
		case BlendFunc::k_one_minus_destination_color:	return GL_ONE_MINUS_DST_COLOR;
		case BlendFunc::k_source_alpha:					return GL_SRC_ALPHA;
		case BlendFunc::k_one_minus_source_alpha:		return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFunc::k_destination_alpha:			return GL_DST_ALPHA;
		case BlendFunc::k_one_minus_destination_alpha:	return GL_ONE_MINUS_DST_ALPHA;
		case BlendFunc::k_constant_color:				return GL_CONSTANT_COLOR;
		case BlendFunc::k_one_minus_constant_color:		return GL_ONE_MINUS_CONSTANT_COLOR;
		case BlendFunc::k_constant_alpha:				return GL_CONSTANT_ALPHA;
		case BlendFunc::k_one_minus_constant_alpha:		return GL_ONE_MINUS_CONSTANT_ALPHA;
		case BlendFunc::k_source_alpha_saturate:		return GL_SRC_ALPHA_SATURATE;
		case BlendFunc::k_source1_color:				return GL_SRC1_COLOR;
		case BlendFunc::k_one_minus_source1_color:		return GL_ONE_MINUS_SRC1_COLOR;
		case BlendFunc::k_source1_alpha:				return GL_SRC1_ALPHA;
		case BlendFunc::k_one_minus_source1_alpha:		return GL_ONE_MINUS_SRC1_ALPHA;
		}
		return GL_ONE;
	}

	GLbitfield TranslateClearBits(ClearBits::value_type clearBits)
	{
		GLbitfield returnClearBits = 0;
		if (clearBits & ClearBits::k_color_buffer_bit)
			returnClearBits |= GL_COLOR_BUFFER_BIT;
		if (clearBits & ClearBits::k_depth_buffer_bit)
			returnClearBits |= GL_DEPTH_BUFFER_BIT;
		if (clearBits & ClearBits::k_stencil_buffer_bit)
			returnClearBits |= GL_STENCIL_BUFFER_BIT;
		return returnClearBits;
	}

	GLenum TranslateFrontFace(FrontFace::value_type frontFace)
	{
		if (frontFace == FrontFace::k_counter_clockwise)
			return GL_CCW;
		else
			return GL_CW;
	}

	GLenum TranslateCullFace(CullFace::value_type cullFace)
	{
		if (cullFace == CullFace::k_back)
			return GL_BACK;
		else if (cullFace == CullFace::k_front)
			return GL_FRONT;
		else
			return GL_FRONT_AND_BACK;
	}

	GLenum TranslatePolygonMode(PolygonMode::value_type polygonMode)
	{
		if (polygonMode == PolygonMode::k_fill)
			return GL_FILL;
		else if (polygonMode == PolygonMode::k_line)
			return GL_LINE;
		else
			return GL_POINT;
	}

	GLenum TranslateVertexPrimitiveType(VertexPrimitiveType::value_type primitiveType)
	{
		switch (primitiveType)
		{
		case VertexPrimitiveType::k_triangles:		return GL_TRIANGLES;
		case VertexPrimitiveType::k_points:			return GL_POINTS;
		case VertexPrimitiveType::k_lines:			return GL_LINES;
		case VertexPrimitiveType::k_line_strip:		return GL_LINE_STRIP;
		case VertexPrimitiveType::k_line_loop:		return GL_LINE_LOOP;
		case VertexPrimitiveType::k_triangle_strip:	return GL_TRIANGLE_STRIP;
		case VertexPrimitiveType::k_triangle_fan:	return GL_TRIANGLE_FAN;
		case VertexPrimitiveType::k_quad_strip:		return GL_QUAD_STRIP;
		case VertexPrimitiveType::k_polygon:		return GL_POLYGON;
		}
		return GL_TRIANGLES;
	}

};

Shader* Renderer::s_defaultShader = nullptr;


Renderer::Renderer() :
	m_activeShader(nullptr),
	m_isShaderChanged(false),
	m_projectionMatrix(Matrix4f::IDENTITY),
	m_viewMatrix(Matrix4f::IDENTITY)
{
}

	
void Renderer::SetShader(Shader* shader)
{
	m_activeShader = shader;
	m_isShaderChanged = true;

	if (m_activeShader != nullptr)
	{
		if (m_activeShader->IsLinked())
			glUseProgram(m_activeShader->GetGLProgram());
		else if (s_defaultShader != nullptr && s_defaultShader->IsLinked())
		{
			m_activeShader = s_defaultShader;
			glUseProgram(s_defaultShader->GetGLProgram());
		}
		else
			glUseProgram(0);
	}
	else if (s_defaultShader != nullptr && s_defaultShader->IsLinked())
	{
		m_activeShader = s_defaultShader;
		glUseProgram(s_defaultShader->GetGLProgram());
	}
	else
		glUseProgram(0);
}

void Renderer::SetDefaultShader(Shader* defaultShader)
{
	s_defaultShader = defaultShader;
}

void Renderer::SetViewMatrix(const Matrix4f& viewMatrix)
{
	m_viewMatrix = viewMatrix;
}

void Renderer::SetProjectionMatrix(const Matrix4f& projectionMatrix)
{
	m_projectionMatrix = projectionMatrix;
}


void Renderer::UpdateUniforms()
{
	if (m_activeShader != nullptr)
	{
		int uniformLocation = -1;
		
		// Color.
		//if (m_activeShader->GetUniformLocation("u_color", uniformLocation))
		//	glUniform4fv(uniformLocation, 1, m_color.data());

		// MVP matrix.
		//Matrix4f mvp = GetMVP();
		//if (m_activeShader->GetUniformLocation("u_mvp", uniformLocation))
		//	glUniformMatrix4fv(uniformLocation, 1, GL_TRUE, mvp.data());
	}
}

void Renderer::ApplyRenderSettings(bool clear)
{
	// Depth.
	EnableDisableGL(m_params.IsDepthTestEnabled(), GL_DEPTH_TEST);
	EnableDisableGL(!m_params.IsNearFarPlaneClippingEnabled(), GL_DEPTH_CLAMP);
	glDepthFunc(TranslateCompareFunc(m_params.GetDepthFunction()));
	glDepthMask(m_params.IsDepthBufferWriteEnabled());				// glDepthMask(true) means writing to the depth buffer is enabled.

	// Face culling.
	EnableDisableGL(m_params.IsCullFaceEnabled(), GL_CULL_FACE);
	glFrontFace(TranslateFrontFace(m_params.GetFrontFace()));
	glCullFace(TranslateCullFace(m_params.GetCullFace()));
		
	// Smoothing.
	EnableDisableGL(m_params.IsLineSmoothEnabled(), GL_LINE_SMOOTH);
	EnableDisableGL(m_params.IsPolygonSmoothEnabled(), GL_POLYGON_SMOOTH);

	// Blend.
	EnableDisableGL(m_params.IsBlendEnabled(), GL_BLEND);
	glBlendFunc(TranslateBlendFunc(m_params.GetBlendFunction().source),
				TranslateBlendFunc(m_params.GetBlendFunction().destination));

	// Polygon mode.
	glPolygonMode(GL_FRONT_AND_BACK, TranslatePolygonMode(m_params.GetPolygonMode()));

	// Clear color.
	Vector4f clearColorVec = m_params.GetClearColor().ToVector4f();
	glClearColor(clearColorVec.x, clearColorVec.y,
				 clearColorVec.z, clearColorVec.w);
	
	if (clear)
		Clear();
}

void Renderer::Clear()
{
	glClear(TranslateClearBits(m_params.GetClearBits()));
}

//void Renderer::SetModelMatrix(const Matrix4f& model)
//{
//	m_modelMatrix = model;
//}
//
//void Renderer::SetViewMatrix(const Matrix4f& view)
//{
//	m_viewMatrix = view;
//}
//
//void Renderer::SetProjectionMatrix(const Matrix4f& projection)
//{
//	m_projectionMatrix = projection;
//}
//
//Matrix4f Renderer::GetMVP()
//{
//	return (m_projectionMatrix * m_viewMatrix * m_modelMatrix);
//}


/*
void Renderer::RenderMesh(Mesh* mesh, Material* material, const Transform3f& transform)
{
	RenderMesh(mesh, material, transform.GetMatrix());
}

void Renderer::RenderMesh(Mesh* mesh, Material* material, const Matrix4f& modelMatrix)
{
	glBindTexture(GL_TEXTURE_2D, 0);

	// Update uniforms.
	if (m_activeShader != nullptr)
	{
		int uniformLocation = -1;
		
		// Material properties.
		if (material != m_material || m_isShaderChanged)
		{
			m_material = material;
			if (m_activeShader->GetUniformLocation("u_isLit", uniformLocation))
				glUniform1i(uniformLocation, (int) material->IsLit());
			if (m_activeShader->GetUniformLocation("u_color", uniformLocation))
				glUniform4fv(uniformLocation, 1, material->GetColor().ToVector4f().data());
		}

		// Lighting properties.
		if (m_isShaderChanged)
		{
			if (m_activeShader->GetUniformLocation("u_lightColor", uniformLocation))
				glUniform3fv(uniformLocation, 1, m_lightColor.ToVector3f().data());
			if (m_activeShader->GetUniformLocation("u_ambientLight", uniformLocation))
				glUniform3fv(uniformLocation, 1, m_ambientLight.ToVector3f().data());
			if (m_activeShader->GetUniformLocation("u_lightDir", uniformLocation))
				glUniform3fv(uniformLocation, 1, m_lightDirection.data());
		}

		// Transform matrices.
		Matrix4f mvp = m_camera->GetViewProjection() * modelMatrix;
		if (m_activeShader->GetUniformLocation("u_mvp", uniformLocation))
			glUniformMatrix4fv(uniformLocation, 1, GL_TRUE, mvp.data());
		if (m_activeShader->GetUniformLocation("u_model", uniformLocation))
			glUniformMatrix4fv(uniformLocation, 1, GL_TRUE, modelMatrix.data());
		
		m_isShaderChanged = false;
	}

	// Bind the mesh.
	if (m_mesh != mesh)
	{
		m_mesh = mesh;
		glBindVertexArray(mesh->GetVertexData()->GetVertexBuffer()->m_glVertexArray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndexData()->GetIndexBuffer()->m_glIndexBuffer);
	}
	
	// Send the draw call to the GPU.
	glDrawElements(TranslateVertexPrimitiveType(mesh->GetPrimitiveType()),
		mesh->GetIndexData()->GetCount(), GL_UNSIGNED_INT, (unsigned int*) 0);
}


Shader* GetDefaultShader()
{
}
*/