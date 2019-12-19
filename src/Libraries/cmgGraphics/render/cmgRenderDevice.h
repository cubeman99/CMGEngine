#ifndef _CMG_GRAPHICS_RENDER_DEVICE_H_
#define _CMG_GRAPHICS_RENDER_DEVICE_H_

#include <cmgCore/cmg_core.h>
#include <cmgGraphics/render/cmgRenderTarget.h>
#include <cmgGraphics/render/cmgSampler.h>
#include <cmgGraphics/cmgSpriteFont.h>
#include <cmgGraphics/cmgTexture.h>
#include <cmgGraphics/cmgShader.h>
#include <cmgGraphics/cmgMesh.h>
#include <cmgGraphics/cmgImageFormat.h>
#include <cmgMath/types/cmgVector3.h>

class Window;


class OpenGLRenderDevice
{
public:
	OpenGLRenderDevice(Window* window);
	~OpenGLRenderDevice();

	Error CreateTexture2D(Texture** outTexture, int32 width, int32 height,
		const TextureParams& params);

	Error CreateShaderProgram(Shader** outShader, const String& vertexCode, const String& fragmentCode);
	Error SetShaderUniform(Shader::sptr shader, const String& name, int32 value);
	Error SetShaderUniform(Shader::sptr shader, const String& name, uint32 value);
	Error SetShaderUniform(Shader::sptr shader, const String& name, float32 value);
	Error SetShaderUniform(Shader::sptr shader, const String& name, const Vector2f& value);
	Error SetShaderUniform(Shader::sptr shader, const String& name, const Vector2i& value);
	Error SetShaderUniform(Shader::sptr shader, const String& name, const Vector2ui& value);
	Error SetShaderUniform(Shader::sptr shader, const String& name, const Vector3f& value);
	Error SetShaderUniform(Shader::sptr shader, const String& name, const Vector3i& value);
	Error SetShaderUniform(Shader::sptr shader, const String& name, const Vector3ui& value);
	Error SetShaderUniform(Shader::sptr shader, const String& name, const Vector4f& value);
	Error SetShaderUniform(Shader::sptr shader, const String& name, const Matrix4f& value);
	Error SetTextureSampler(Shader::sptr shader, const String& name,
		Texture::sptr texture, uint32 slot);
	Error SetShaderSampler(Shader::sptr shader, const String& samplerName,
		Texture::sptr texture, Sampler* sampler, uint32 slot);

	void BindBuffer(const BufferObject& buffer, uint32 slot);

	Error CreateRenderTarget(RenderTarget** outRenderTarget);


	void Clear(RenderTarget* target, const Color& color, bool clearDepth);
	void Draw(RenderTarget* target, Shader::sptr shader, Mesh::sptr mesh);
	void DispatchCompute(Shader::sptr shader, Vector2ui numGroupsXY);
	void DispatchCompute(Shader::sptr shader, Vector3ui numGroupsXYZ);
	void DispatchCompute(Shader::sptr shader, uint32 numGroupsX, uint32 numGroupsY = 1, uint32 numGroupsZ = 1);

private:
	void SetRenderTarget(RenderTarget* renderTarget);
	void SetViewport(RenderTarget* renderTarget);
	void SetShader(Shader::sptr shader);

	Array<RenderTarget*> m_renderTargets;
	Array<Texture*> m_textures;
	Array<Font*> m_fonts;
	Array<Shader*> m_shaders;
	Array<VertexBuffer*> m_vertexBuffers;
	Array<IndexBuffer*> m_indexBuffers;
	Window* m_window;
};


typedef OpenGLRenderDevice RenderDevice;


#endif // _CMG_GRAPHICS_RENDER_DEVICE_H_