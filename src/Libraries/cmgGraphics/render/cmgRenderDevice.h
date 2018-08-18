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

class Window;


class OpenGLRenderDevice
{
public:
	OpenGLRenderDevice(Window* window);
	~OpenGLRenderDevice();

	Error CreateTexture2D(Texture** outTexture, int32 width, int32 height,
		const TextureParams& params);

	Error CreateShaderProgram(Shader** outShader, const String& vertexCode, const String& fragmentCode);
	Error SetShaderUniform(Shader* shader, const String& name, float value);
	Error SetShaderUniform(Shader* shader, const String& name, const Vector2f& value);
	Error SetShaderUniform(Shader* shader, const String& name, const Vector3f& value);
	Error SetShaderUniform(Shader* shader, const String& name, const Vector4f& value);
	Error SetShaderUniform(Shader* shader, const String& name, const Matrix4f& value);
	Error SetShaderSampler(Shader* shader, const String& samplerName,
		Texture* texture, Sampler* sampler, uint32 slot);

	Error CreateRenderTarget(RenderTarget** outRenderTarget);


	void Clear(RenderTarget* target, const Color& color, bool clearDepth);
	void Draw(RenderTarget* target, Shader* shader, Mesh* mesh);

private:
	void SetRenderTarget(RenderTarget* renderTarget);
	void SetViewport(RenderTarget* renderTarget);
	void SetShader(Shader* shader);

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