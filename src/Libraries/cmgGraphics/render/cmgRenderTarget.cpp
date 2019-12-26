#include "cmgRenderTarget.h"
#include <cmgGraphics/cmgOpenGLIncludes.h>
#include <cmgGraphics/cmgTexture.h>
#include <cmgGraphics/render/cmgRenderDevice.h>


RenderTarget::RenderTarget(OpenGLRenderDevice* device)
	: m_device(device)
{
	glGenFramebuffers(1, &m_glFrameBufferId);
	memset(m_colorAttachments, 0, sizeof(m_colorAttachments));
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

void RenderTarget::AddColorAttachment(uint32 location, Texture* texture)
{
	m_colorAttachments[location] = texture;
	m_width = texture->GetWidth();
	m_height = texture->GetHeight();

	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBufferId);
	glFramebufferTexture2D(GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + location,
		GL_TEXTURE_2D, texture->m_glTextureId, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	UpdateDrawBuffers();
}

void RenderTarget::AddColorAttachment(uint32 location, uint32 width, uint32 height, const TextureParams& params)
{
	Texture* texture = nullptr;
	m_device->CreateTexture2D(texture, (int32) width, (int32) height, params);
	AddColorAttachment(0, texture);
}

void RenderTarget::UpdateDrawBuffers()
{
	unsigned int drawBuffers[16];
	unsigned int i;

	// Create a list of color targets.
	uint32 numDrawTargets = 0;
	for (i = 0; i < 16; ++i)
	{
		if (m_colorAttachments[i] != nullptr)
		{
			drawBuffers[numDrawTargets] = GL_COLOR_ATTACHMENT0 + i;
			numDrawTargets++;
		}
	}

	// Assign the draw buffers.
	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBufferId);
	glDrawBuffers(numDrawTargets, drawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


