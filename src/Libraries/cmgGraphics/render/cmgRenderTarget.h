#ifndef _CMG_GRAPHICS_RENDER_TARGET_H_
#define _CMG_GRAPHICS_RENDER_TARGET_H_

#include <cmgCore/cmgBase.h>
#include <cmgGraphics/cmgTexture.h>

// A frame buffer
class RenderTarget
{
public:
	using sptr = cmg::shared_ptr<RenderTarget>;

public:
	static constexpr uint32 MAX_ATTACHMENTS = 16;

	RenderTarget(uint32 width, uint32 height);
	virtual ~RenderTarget();
	
	// Getters
	uint32 GetWidth() const;
	uint32 GetHeight() const;
	Texture::sptr GetColorAttachment(uint32 location);
	Texture::sptr GetDepthAttachment();
	Texture::sptr GetStencilAttachment();
	Texture::sptr GetDepthStencilAttachment();
	
	// Attachment modifiers
	void AddColorAttachment(uint32 location, Texture::sptr texture);
	void AddDepthAttachment(Texture::sptr texture);
	Texture::sptr CreateColorAttachment(uint32 location);
	Texture::sptr CreateColorAttachment(uint32 location, const TextureParams& params);
	Texture::sptr CreateCubeMapColorAttachments(uint32 firstLocation);
	Texture::sptr CreateCubeMapColorAttachments(uint32 firstLocation, TextureParams params);
	Texture::sptr CreateDepthAttachment(
		PixelInternalFormat format = PixelInternalFormat::k_depth_32f);
	Texture::sptr CreateStencilAttachment(
		PixelInternalFormat format = PixelInternalFormat::k_stencil_index);
	Texture::sptr CreateDepthStencilAttachment(
		PixelInternalFormat format = PixelInternalFormat::k_depth24_stencil8);

	void SetColorTargets(const Array<uint32>& colorTargets);

private:
	// Auto set draw-targets to all color attachments sequentually
	void UpdateDrawBuffers();

public:
	// Prevent copying
	RenderTarget(const RenderTarget& other) {}
	void operator=(const RenderTarget& other) {}

	uint32 m_width;
	uint32 m_height;

	Texture::sptr m_colorAttachments[MAX_ATTACHMENTS];

	// If using a single depth-stencil attachment, then both depth and
	// stencil will point to the same texture
	Texture::sptr m_depthAttachment;
	Texture::sptr m_stencilAttachment;

	uint32 m_glFrameBufferId;
};


#endif // _CMG_GRAPHICS_RENDER_TARGET_H_