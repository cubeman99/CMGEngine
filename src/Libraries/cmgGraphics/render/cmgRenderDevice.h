#ifndef _CMG_GRAPHICS_RENDER_DEVICE_H_
#define _CMG_GRAPHICS_RENDER_DEVICE_H_

#include <cmgCore/cmg_core.h>
#include <cmgGraphics/render/cmgRenderTarget.h>
#include <cmgGraphics/render/cmgSampler.h>
#include <cmgGraphics/cmgTexture.h>
#include <cmgGraphics/cmgShader.h>
#include <cmgGraphics/cmgMesh.h>
#include <cmgGraphics/render/cmgRenderParams.h>
#include <cmgGraphics/types/cmgImageFormat.h>
#include <cmgMath/types/cmgVector3.h>

class Window;


class OpenGLRenderDevice
{
public:
	OpenGLRenderDevice(Window* window);
	~OpenGLRenderDevice();

	// Render parameters
	const RenderParams& GetRenderParams() const;
	void SetRenderParams(const RenderParams& renderParams);
	void ApplyRenderSettings(bool clear = false);

	// Uniforms
	Error SetShaderUniform(Shader* shader, const String& name, int32 value);
	Error SetShaderUniform(Shader* shader, const String& name, uint32 value);
	Error SetShaderUniform(Shader* shader, const String& name, float32 value);
	Error SetShaderUniform(Shader* shader, const String& name, const Vector2f& value);
	Error SetShaderUniform(Shader* shader, const String& name, const Vector2i& value);
	Error SetShaderUniform(Shader* shader, const String& name, const Vector2ui& value);
	Error SetShaderUniform(Shader* shader, const String& name, const Vector3f& value);
	Error SetShaderUniform(Shader* shader, const String& name, const Vector3i& value);
	Error SetShaderUniform(Shader* shader, const String& name, const Vector3ui& value);
	Error SetShaderUniform(Shader* shader, const String& name, const Vector4f& value);
	Error SetShaderUniform(Shader* shader, const String& name, uint32 count, const Matrix4f* data);
	Error SetShaderUniform(Shader* shader, const String& name, const Matrix4f& value);
	Error SetTextureSampler(Shader* shader, const String& name,
		Texture* texture, uint32 slot);
	Error SetShaderSampler(Shader* shader, const String& samplerName,
		Texture* texture, Sampler* sampler, uint32 slot);

	void BindBuffer(const BufferObject& buffer, uint32 slot);

	// Drawing
	void ClearStencilBuffer(RenderTarget* target, int32 value = 0);
	void ClearDepthBuffer(RenderTarget* target, float value = 1.0f);
	void ClearColorBuffer(RenderTarget* target, const Color& color);
	void ClearColorAndDepth(RenderTarget* target, const Color& color, float clearDepth = 1.0f);
	void Clear(RenderTarget* target, const Color& color, bool clearDepth);
	void Draw(RenderTarget* target, Shader* shader, Mesh* mesh);
	void DispatchCompute(Shader* shader, Vector2ui numGroupsXY);
	void DispatchCompute(Shader* shader, Vector3ui numGroupsXYZ);
	void DispatchCompute(Shader* shader, uint32 numGroupsX, uint32 numGroupsY = 1, uint32 numGroupsZ = 1);

private:
	void SetRenderTarget(RenderTarget* renderTarget);
	void SetViewport(RenderTarget* renderTarget);
	void SetShader(Shader* shader);

	Window* m_window;
	RenderParams m_renderParams;

	Array<RenderTarget*> m_renderTargets;
	Array<Texture*> m_textures;
	Array<Font*> m_fonts;
	Array<Shader*> m_shaders;
	Array<VertexBuffer*> m_vertexBuffers;
	Array<IndexBuffer*> m_indexBuffers;
};


typedef OpenGLRenderDevice RenderDevice;


#endif // _CMG_GRAPHICS_RENDER_DEVICE_H_