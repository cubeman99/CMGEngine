#ifndef _CMG_GRAPHICS_RENDER_TARGET_H_
#define _CMG_GRAPHICS_RENDER_TARGET_H_

#include <cmgCore/cmgBase.h>
#include <cmgGraphics/cmgTexture.h>


class RenderTarget
{
	friend class OpenGLRenderDevice;

public:
	static const uint32 MAX_ATTACHMENTS = 16;

	RenderTarget(OpenGLRenderDevice* device);
	~RenderTarget();
	
	// Getters
	uint32 GetWidth() const;
	uint32 GetHeight() const;
	
	void AddColorAttachment(uint32 location, Texture* texture);
	void AddColorAttachment(uint32 location, uint32 width, uint32 height, const TextureParams& params);

	inline Texture* GetColorAttachment(uint32 location)
	{
		return m_colorAttachments[location];
	}

private:
	void UpdateDrawBuffers();

public:
	OpenGLRenderDevice* m_device;
	uint32 m_glFrameBufferId;
	uint32 m_width;
	uint32 m_height;
	Texture* m_colorAttachments[MAX_ATTACHMENTS];
};


#endif // _CMG_GRAPHICS_RENDER_TARGET_H_