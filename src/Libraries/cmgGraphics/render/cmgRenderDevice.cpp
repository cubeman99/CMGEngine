#include "cmgRenderDevice.h"
#include <cmgGraphics/cmgOpenGLIncludes.h>
#include <cmgGraphics/cmgWindow.h>


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
}



OpenGLRenderDevice::OpenGLRenderDevice(Window* window)
	: m_window(window)
{
}

OpenGLRenderDevice::~OpenGLRenderDevice()
{
}

void OpenGLRenderDevice::ApplyRenderSettings(bool clear)
{
	glEnable(GL_TEXTURE_2D);

	// Depth
	EnableDisableGL(m_renderParams.IsDepthTestEnabled(), GL_DEPTH_TEST);
	EnableDisableGL(!m_renderParams.IsNearFarPlaneClippingEnabled(), GL_DEPTH_CLAMP);
	glDepthFunc(TranslateCompareFunc(m_renderParams.GetDepthFunction()));
	glDepthMask(m_renderParams.IsDepthBufferWriteEnabled());				// glDepthMask(true) means writing to the depth buffer is enabled.

																	// Face culling
	EnableDisableGL(m_renderParams.IsCullFaceEnabled(), GL_CULL_FACE);
	glFrontFace(TranslateFrontFace(m_renderParams.GetFrontFace()));
	glCullFace(TranslateCullFace(m_renderParams.GetCullFace()));

	// Smoothing
	EnableDisableGL(m_renderParams.IsLineSmoothEnabled(), GL_LINE_SMOOTH);
	EnableDisableGL(m_renderParams.IsPolygonSmoothEnabled(), GL_POLYGON_SMOOTH);

	// Blend
	EnableDisableGL(m_renderParams.IsBlendEnabled(), GL_BLEND);
	glBlendFunc(TranslateBlendFunc(m_renderParams.GetBlendFunction().source),
		TranslateBlendFunc(m_renderParams.GetBlendFunction().destination));

	// Polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, TranslatePolygonMode(m_renderParams.GetPolygonMode()));

	// Clear color
	Vector4f clearColorVec = m_renderParams.GetClearColor().ToVector4f();
	glClearColor(clearColorVec.x, clearColorVec.y,
		clearColorVec.z, clearColorVec.w);

	if (clear)
		glClear(TranslateClearBits(m_renderParams.GetClearBits()));
}

Error OpenGLRenderDevice::CreateTexture2D(Texture*& outTexture, int32 width,
	int32 height, const TextureParams& params)
{
	outTexture = new Texture();
	outTexture->SetParams(params);
	outTexture->WritePixels2D(width, height, PixelTransferFormat::RGBA,
		PixelType::TYPE_UNSIGNED_BYTE, nullptr);
	return CMG_ERROR_SUCCESS;
}

Error OpenGLRenderDevice::CreateShaderProgram(Shader** outShader,
	const String& vertexCode, const String& fragmentCode)
{
	Shader* shader = new Shader();
	shader->AddStage(ShaderType::k_vertex_shader, vertexCode, "vertex shader");
	shader->AddStage(ShaderType::k_fragment_shader, fragmentCode, "fragment shader");

	Error shaderError = shader->CompileAndLink();
	if (shaderError.Failed())
	{
		*outShader = nullptr;
		std::cout << "Shader Error:/n" << shaderError.GetText() << std::endl;
		delete shader;
	}
	else
	{
		*outShader = shader;
	}

	return shaderError;
}

Error OpenGLRenderDevice::SetShaderUniform(Shader::sptr shader, const String& name, int32 value)
{
	int32 uniformLocation;
	SetShader(shader);
	if (shader->GetUniformLocation(name, uniformLocation))
	{
		glUniform1i(uniformLocation, value);
		return CMG_ERROR_SUCCESS;
	}
	else
	{
		return CMG_ERROR_FAILURE;
	}
}

Error OpenGLRenderDevice::SetShaderUniform(Shader::sptr shader, const String& name, uint32 value)
{
	int32 uniformLocation;
	SetShader(shader);
	if (shader->GetUniformLocation(name, uniformLocation))
	{
		glUniform1ui(uniformLocation, value);
		return CMG_ERROR_SUCCESS;
	}
	else
	{
		return CMG_ERROR_FAILURE;
	}
}

Error OpenGLRenderDevice::SetShaderUniform(Shader::sptr shader, const String& name, float32 value)
{
	int32 uniformLocation;
	SetShader(shader);
	if (shader->GetUniformLocation(name, uniformLocation))
	{
		glUniform1f(uniformLocation, value);
		return CMG_ERROR_SUCCESS;
	}
	else
	{
		return CMG_ERROR_FAILURE;
	}
}

Error OpenGLRenderDevice::SetShaderUniform(Shader::sptr shader, const String& name, const Vector2f& value)
{
	int32 uniformLocation;
	SetShader(shader);
	if (shader->GetUniformLocation(name, uniformLocation))
	{
		glUniform2fv(uniformLocation, 1, value.v);
		return CMG_ERROR_SUCCESS;
	}
	else
	{
		return CMG_ERROR_FAILURE;
	}
}

Error OpenGLRenderDevice::SetShaderUniform(Shader::sptr shader, const String& name, const Vector2ui& value)
{
	int32 uniformLocation;
	SetShader(shader);
	if (shader->GetUniformLocation(name, uniformLocation))
	{
		glUniform2uiv(uniformLocation, 1, value.v);
		return CMG_ERROR_SUCCESS;
	}
	else
	{
		return CMG_ERROR_FAILURE;
	}
}

Error OpenGLRenderDevice::SetShaderUniform(Shader::sptr shader, const String& name, const Vector2i& value)
{
	int32 uniformLocation;
	SetShader(shader);
	if (shader->GetUniformLocation(name, uniformLocation))
	{
		glUniform2iv(uniformLocation, 1, value.v);
		return CMG_ERROR_SUCCESS;
	}
	else
	{
		return CMG_ERROR_FAILURE;
	}
}

Error OpenGLRenderDevice::SetShaderUniform(Shader::sptr shader, const String& name, const Vector3f& value)
{
	int32 uniformLocation;
	SetShader(shader);
	if (shader->GetUniformLocation(name, uniformLocation))
	{
		glUniform3fv(uniformLocation, 1, value.v);
		return CMG_ERROR_SUCCESS;
	}
	else
	{
		return CMG_ERROR_FAILURE;
	}
}

Error OpenGLRenderDevice::SetShaderUniform(Shader::sptr shader, const String& name, const Vector3i& value)
{
	int32 uniformLocation;
	SetShader(shader);
	if (shader->GetUniformLocation(name, uniformLocation))
	{
		glUniform3iv(uniformLocation, 1, value.v);
		return CMG_ERROR_SUCCESS;
	}
	else
	{
		return CMG_ERROR_FAILURE;
	}
}

Error OpenGLRenderDevice::SetShaderUniform(Shader::sptr shader, const String& name, const Vector3ui& value)
{
	int32 uniformLocation;
	SetShader(shader);
	if (shader->GetUniformLocation(name, uniformLocation))
	{
		glUniform3uiv(uniformLocation, 1, value.v);
		return CMG_ERROR_SUCCESS;
	}
	else
	{
		return CMG_ERROR_FAILURE;
	}
}

Error OpenGLRenderDevice::SetShaderUniform(Shader::sptr shader, const String& name, const Vector4f& value)
{
	int32 uniformLocation;
	SetShader(shader);
	if (shader->GetUniformLocation(name, uniformLocation))
	{
		glUniform4fv(uniformLocation, 1, value.v);
		return CMG_ERROR_SUCCESS;
	}
	else
	{
		return CMG_ERROR_FAILURE;
	}
}

Error OpenGLRenderDevice::SetShaderUniform(Shader::sptr shader, const String& name, const Matrix4f& value)
{
	int32 uniformLocation;
	SetShader(shader);
	if (shader->GetUniformLocation(name, uniformLocation))
	{
		glUniformMatrix4fv(uniformLocation, 1, false, value.m);
		return CMG_ERROR_SUCCESS;
	}
	else
	{
		return CMG_ERROR_FAILURE;
	}
}

Error OpenGLRenderDevice::SetTextureSampler(Shader::sptr shader,
	const String& name, Texture* texture, uint32 slot)
{
	SetShader(shader);
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture->m_glTextureId);
	return SetShaderUniform(shader, name, (int) slot);
}

Error OpenGLRenderDevice::SetShaderSampler(Shader::sptr shader,
	const String& samplerName, Texture* texture, Sampler* sampler, uint32 slot)
{
	SetShader(shader);
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture->m_glTextureId);
	glBindSampler(slot, sampler->GetId());

	int32 uniformLocation;
	if (shader->GetUniformLocation(samplerName, uniformLocation))
	{
		glUniform1i(uniformLocation, slot);
		return CMG_ERROR_SUCCESS;
	}
	else
	{
		return CMG_ERROR_FAILURE;
	}
}

void OpenGLRenderDevice::BindBuffer(const BufferObject & buffer, uint32 slot)
{
	glBindBufferBase(buffer.GetGLTarget(), slot, buffer.GetGLBuffer());
}


Error OpenGLRenderDevice::CreateRenderTarget(RenderTarget** outRenderTarget)
{
	*outRenderTarget = new RenderTarget(this);
	return CMG_ERROR_SUCCESS;
}

//RenderTarget* RenderTarget::CreateRenderTarget(Texture* texture)
//{
//	RenderTarget* renderTarget = new RenderTarget();
//	renderTarget->m_width = texture->GetWidth();
//	renderTarget->m_height = texture->GetHeight();
//	renderTarget->AddColorAttachment(0, texture);
//	return renderTarget;
//}


void OpenGLRenderDevice::Clear(RenderTarget* target, const Color& color,
	bool clearDepth)
{
	SetRenderTarget(target);
	SetViewport(target);
	Vector4f v = color.ToVector4f();
	glClearColor(v.x, v.y, v.z, v.w);
	uint32 flags = GL_COLOR_BUFFER_BIT;
	if (clearDepth)
		flags |= GL_DEPTH_BUFFER_BIT;
	glClear(flags);
}

void OpenGLRenderDevice::Draw(RenderTarget* target, Shader::sptr shader, Mesh::sptr mesh)
{
	SetRenderTarget(target);
	SetViewport(target);
	SetShader(shader);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVertexData()->
		GetVertexBuffer()->GetGLVertexBuffer());
	glBindVertexArray(mesh->GetVertexData()->
		GetVertexBuffer()->GetGLVertexArray());

	if (mesh->GetIndexData()->GetCount() == 0)
	{
		// Draw non-indexed polygons
		if (mesh->GetNumIndices() > 0)
		{
			glDrawArrays(GL_TRIANGLES, mesh->GetFirstIndex(),
				mesh->GetNumIndices());
		}
	}
	else
	{
		uint32 start, count;
		if (mesh->GetNumIndices() == 0)
		{
			start = mesh->GetIndexData()->GetStart();
			count = mesh->GetIndexData()->GetCount();
		}
		else
		{
			start = mesh->GetFirstIndex();
			count = mesh->GetNumIndices();
		}
		// Draw indexed polygons
		if (count > 0)
		{
			uint32 glPrimitiveType = GL_TRIANGLES;
			if (mesh->GetPrimitiveType() == VertexPrimitiveType::k_lines)
				glPrimitiveType = GL_LINES;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
				mesh->GetIndexData()->GetIndexBuffer()->GetGLBuffer());
			glDrawElements(glPrimitiveType, count,
				GL_UNSIGNED_INT, ((unsigned int*) 0) + start);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}

	glBindVertexArray(0);
}

void OpenGLRenderDevice::DispatchCompute(Shader::sptr shader, Vector2ui numGroupsXY)
{
	DispatchCompute(shader, numGroupsXY.x, numGroupsXY.y, 1);
}

void OpenGLRenderDevice::DispatchCompute(Shader::sptr shader, Vector3ui numGroupsXYZ)
{
	DispatchCompute(shader, numGroupsXYZ.x, numGroupsXYZ.y, numGroupsXYZ.z);
}

void OpenGLRenderDevice::DispatchCompute(Shader::sptr shader, uint32 numGroupsX, uint32 numGroupsY, uint32 numGroupsZ)
{
	SetShader(shader);
	glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void OpenGLRenderDevice::SetRenderTarget(RenderTarget* renderTarget)
{
	if (renderTarget != nullptr)
		glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->m_glFrameBufferId);
	else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLRenderDevice::SetViewport(RenderTarget* renderTarget)
{
	if (renderTarget != nullptr)
		glViewport(0, 0, renderTarget->GetWidth(), renderTarget->GetHeight());
	else
		glViewport(0, 0, m_window->GetWidth(), m_window->GetHeight());
}

void OpenGLRenderDevice::SetShader(Shader::sptr shader)
{
	if (shader != nullptr)
		glUseProgram(shader->m_glProgram);
	else
		glUseProgram(0);
}
