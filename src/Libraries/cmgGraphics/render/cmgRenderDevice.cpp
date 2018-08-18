#include "cmgRenderDevice.h"
#include <cmgGraphics/cmgOpenGLIncludes.h>
#include <cmgGraphics/cmgWindow.h>


OpenGLRenderDevice::OpenGLRenderDevice(Window* window)
	: m_window(window)
{
}

OpenGLRenderDevice::~OpenGLRenderDevice()
{
}


Error OpenGLRenderDevice::CreateTexture2D(Texture** outTexture, int32 width,
	int32 height, const TextureParams& params)
{
	Texture* texture = new Texture();
	texture->SetParams(params);
	texture->WritePixels2D(width, height, PixelTransferFormat::RGBA,
		PixelType::TYPE_UNSIGNED_BYTE, nullptr);
	*outTexture = texture;
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

Error OpenGLRenderDevice::SetShaderUniform(Shader* shader, const String& name, float value)
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

Error OpenGLRenderDevice::SetShaderUniform(Shader* shader, const String& name, const Vector2f& value)
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

Error OpenGLRenderDevice::SetShaderUniform(Shader* shader, const String& name, const Vector3f& value)
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

Error OpenGLRenderDevice::SetShaderUniform(Shader* shader, const String& name, const Vector4f& value)
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

Error OpenGLRenderDevice::SetShaderUniform(Shader* shader, const String& name, const Matrix4f& value)
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

Error OpenGLRenderDevice::SetShaderSampler(Shader* shader,
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

void OpenGLRenderDevice::Draw(RenderTarget* target, Shader* shader, Mesh* mesh)
{
	SetRenderTarget(target);
	SetViewport(target);
	SetShader(shader);
	glBindVertexArray(mesh->GetVertexData()->
		GetVertexBuffer()->GetGLVertexArray());

	if (mesh->GetIndexData()->GetCount() == 0)
	{
		// Draw non-indexed polygons
		glDrawArrays(GL_TRIANGLES, mesh->GetFirstIndex(),
			mesh->GetNumIndices());
	}
	else
	{
		// Draw indexed polygons
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
			mesh->GetIndexData()->GetIndexBuffer()->GetGLIndexBuffer());
		glDrawElements(GL_TRIANGLES, mesh->GetNumIndices(),
			GL_UNSIGNED_INT, ((unsigned int*) 0) + mesh->GetFirstIndex());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(0);
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

void OpenGLRenderDevice::SetShader(Shader* shader)
{
	if (shader != nullptr)
		glUseProgram(shader->m_glProgram);
	else
		glUseProgram(0);
}
