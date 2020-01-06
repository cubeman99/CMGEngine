#include "cmgRenderTarget.h"
#include <cmgGraphics/cmgOpenGLIncludes.h>
#include <cmgGraphics/cmgTexture.h>
#include <cmgGraphics/render/cmgRenderDevice.h>


RenderTarget::RenderTarget(uint32 width, uint32 height) :
	m_width(width),
	m_height(height)
{
	glGenFramebuffers(1, &m_glFrameBufferId);
}

RenderTarget::~RenderTarget()
{
	glDeleteFramebuffers(1, &m_glFrameBufferId);
	m_glFrameBufferId = 0;
}

uint32 RenderTarget::GetWidth() const
{
	return m_width;
}

uint32 RenderTarget::GetHeight() const
{
	return m_height;
}

void RenderTarget::AddColorAttachment(uint32 location, Texture::sptr texture)
{
	m_colorAttachments[location] = texture;
	m_width = texture->GetWidth();
	m_height = texture->GetHeight();

	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBufferId);
	glFramebufferTexture2D(GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + location,
		GL_TEXTURE_2D, texture->GetGLTextureID(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	UpdateDrawBuffers();
}

void RenderTarget::AddDepthAttachment(Texture::sptr texture)
{
	m_depthAttachment = texture;
	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBufferId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, m_depthAttachment->GetGLTextureID(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture::sptr RenderTarget::CreateColorAttachment(uint32 location)
{
	return CreateColorAttachment(location,
		TextureParams(TextureTarget::k_texture_2d));
}

Texture::sptr RenderTarget::CreateColorAttachment(uint32 location,
	const TextureParams& params)
{
	Texture::sptr texture = cmg::make_shared<Texture>(params);
	texture->WritePixels2D(m_width, m_height, PixelTransferFormat::k_rgba,
		PixelType::k_unsigned_byte, nullptr);
	AddColorAttachment(location, texture);
	return texture;
}

Texture::sptr RenderTarget::CreateCubeMapColorAttachments(uint32 firstLocation)
{
	return CreateCubeMapColorAttachments(firstLocation,
		TextureParams(TextureTarget::k_texture_cube_map));
}

Texture::sptr RenderTarget::CreateCubeMapColorAttachments(
	uint32 firstLocation, TextureParams params)
{
	params.SetTarget(TextureTarget::k_texture_cube_map);
	params.SetFiltering(TextureFilter::k_nearest);
	Texture::sptr texture = cmg::make_shared<Texture>(params);
	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBufferId);
	for (uint32 i = 0; i < 6; i++)
	{
		texture->WriteFacePixels(i, m_width, m_height, PixelTransferFormat::k_rgba,
			PixelType::k_unsigned_byte, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + firstLocation + i,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texture->GetGLTextureID(), 0);
		m_colorAttachments[firstLocation + i] = texture;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	UpdateDrawBuffers();
	return texture;
}

Texture::sptr RenderTarget::CreateDepthAttachment(PixelInternalFormat format)
{
	// Create the 2D depth texture
	TextureParams params(TextureTarget::k_texture_2d);
	params.SetInternalFormat(format);
	params.SetFiltering(TextureFilter::k_bilinear);
	Texture::sptr texture = cmg::make_shared<Texture>(params);
	texture->WritePixels2D(m_width, m_height,
		PixelTransferFormat::k_depth_component,
		PixelType::k_unsigned_byte, nullptr);
	AddDepthAttachment(texture);
	return texture;
}

Texture::sptr RenderTarget::CreateStencilAttachment(PixelInternalFormat format)
{
	// Create the 2D stencil texture
	TextureParams params(TextureTarget::k_texture_2d);
	params.SetInternalFormat(format);
	params.SetFiltering(TextureFilter::k_bilinear);
	m_stencilAttachment = cmg::make_shared<Texture>(params);
	m_stencilAttachment->WritePixels2D(m_width, m_height,
		PixelTransferFormat::k_stencil_index,
		PixelType::k_unsigned_byte, nullptr);

	// Attach the stencil texture
	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBufferId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, m_stencilAttachment->GetGLTextureID(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return m_stencilAttachment;
}

Texture::sptr RenderTarget::CreateDepthStencilAttachment(PixelInternalFormat format)
{
	// Create the 2D depth-stencil texture
	TextureParams params(TextureTarget::k_texture_2d);
	params.SetInternalFormat(format);
	params.SetFiltering(TextureFilter::k_bilinear);
	m_depthAttachment = cmg::make_shared<Texture>(params);
	m_depthAttachment->WritePixels2D(m_width, m_height,
		PixelTransferFormat::k_depth_stencil,
		PixelType::k_unsigned_byte, nullptr);

	// Attach the depth-stencil texture
	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBufferId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, m_stencilAttachment->GetGLTextureID(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_stencilAttachment = m_depthAttachment;
	return m_depthAttachment;
}

void RenderTarget::SetColorTargets(const Array<uint32>& colorTargets)
{
	uint32 drawBuffers[MAX_ATTACHMENTS];

	// Create a list of color targets
	uint32 numDrawTargets = Math::Min(MAX_ATTACHMENTS, colorTargets.size());
	for (uint32 i = 0; i < numDrawTargets; i++)
		drawBuffers[i] = GL_COLOR_ATTACHMENT0 + colorTargets[i];

	// Assign the draw buffers
	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBufferId);
	glDrawBuffers(numDrawTargets, drawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture::sptr RenderTarget::GetColorAttachment(uint32 location)
{
	return m_colorAttachments[location];
}

Texture::sptr RenderTarget::GetDepthAttachment()
{
	return m_depthAttachment;
}

Texture::sptr RenderTarget::GetStencilAttachment()
{
	return m_stencilAttachment;
}

Texture::sptr RenderTarget::GetDepthStencilAttachment()
{
	return m_depthAttachment;
}

void RenderTarget::UpdateDrawBuffers()
{
	uint32 drawBuffers[MAX_ATTACHMENTS];

	// Create a list of color targets
	uint32 numDrawTargets = 0;
	for (uint32 i = 0; i < MAX_ATTACHMENTS; i++)
	{
		if (m_colorAttachments[i] != nullptr)
		{
			drawBuffers[numDrawTargets] = GL_COLOR_ATTACHMENT0 + i;
			numDrawTargets++;
		}
	}

	// Assign the draw buffers
	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBufferId);
	glDrawBuffers(numDrawTargets, drawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


