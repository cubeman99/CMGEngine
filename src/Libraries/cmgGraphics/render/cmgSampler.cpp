#include "cmgSampler.h"
#include <cmgGraphics/cmgOpenGLIncludes.h>
#include <cmgGraphics/cmgTexture.h>
#include <cmgMath/cmgMathLib.h>

//-----------------------------------------------------------------------------
// OpenGL Enum Conversions
//-----------------------------------------------------------------------------
namespace
{
GLenum TranslateTextureTarget(TextureTarget target)
{
	switch (target)
	{
	case TextureTarget::k_texture_2d:
		return GL_TEXTURE_2D;
	case TextureTarget::k_texture_3d:
		return GL_TEXTURE_3D;
	case TextureTarget::k_texture_1d:
		return GL_TEXTURE_1D;
	case TextureTarget::k_texture_cube_map:
		return GL_TEXTURE_CUBE_MAP;
	}
	CMG_ASSERT_FALSE("Invalid texture target");
	return GL_TEXTURE_2D;
}

GLenum TranslateTextureWrap(TextureWrap wrap)
{
	switch (wrap)
	{
	case TextureWrap::k_clamp_to_border:
		return GL_CLAMP_TO_BORDER;
	case TextureWrap::k_clamp_to_edge:
		return GL_CLAMP_TO_EDGE;
	case TextureWrap::k_repeat:
		return GL_REPEAT;
	case TextureWrap::k_mirror_repeat:
		return GL_MIRRORED_REPEAT;
		//case TextureWrap::MIRROR_CLAMP_TO_EDGE:
		//return GL_MIRROR_CLAMP_TO_EDGE; //TODO: Figure this out.
	}
	CMG_ASSERT_FALSE("Invalid texture wrap");
	return GL_CLAMP_TO_BORDER;
}

GLenum TranslateMinMagFilter(TextureFilterOptions minMagFilter,
	TextureFilterOptions mipmapFilter)
{
	//CMG_ASSERT(minMagFilter != TextureFilterOptions::k_none,
	//"You must specify a min and mag filter!");

	if (minMagFilter == TextureFilterOptions::k_linear)
	{
		if (mipmapFilter == TextureFilterOptions::k_none)
			return GL_LINEAR;
		else if (mipmapFilter == TextureFilterOptions::k_linear)
			return GL_LINEAR_MIPMAP_LINEAR;
		else if (mipmapFilter == TextureFilterOptions::k_nearest)
			return GL_LINEAR_MIPMAP_NEAREST;
	}
	else if (minMagFilter == TextureFilterOptions::k_nearest)
	{
		if (mipmapFilter == TextureFilterOptions::k_none)
			return GL_NEAREST;
		else if (mipmapFilter == TextureFilterOptions::k_linear)
			return GL_NEAREST_MIPMAP_LINEAR;
		else if (mipmapFilter == TextureFilterOptions::k_nearest)
			return GL_NEAREST_MIPMAP_NEAREST;
	}

	return GL_LINEAR;
}
}

Sampler::Sampler(OpenGLRenderDevice& device, const TextureParams& params)
{
	glGenSamplers(1, &m_deviceId);

	// Clamping.
	glSamplerParameteri(m_deviceId, GL_TEXTURE_WRAP_S,
		TranslateTextureWrap(params.GetWrapS()));
	glSamplerParameteri(m_deviceId, GL_TEXTURE_WRAP_T,
		TranslateTextureWrap(params.GetWrapT()));
	glSamplerParameteri(m_deviceId, GL_TEXTURE_WRAP_R,
		TranslateTextureWrap(params.GetWrapR()));
		
	// Min-Mag Filtering.
	auto minFilter = params.GetMinFilter();
	auto magFilter = params.GetMagFilter();
	auto mipFilter = params.GetMipMapFilter();
	glSamplerParameteri(m_deviceId, GL_TEXTURE_MIN_FILTER,
		TranslateMinMagFilter(minFilter, mipFilter));
	glSamplerParameteri(m_deviceId, GL_TEXTURE_MAG_FILTER,
		TranslateMinMagFilter(magFilter, mipFilter));

	// Anisotropic filtering.
	if (params.UseAnisotropy())
	{
		// TODO: Customize anisotropy samples.
		// Determine hardware's max anisotropy samples.
		//glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_anisotropyAmount);
		//m_anisotropyAmount = Math::Clamp(m_anisotropyAmount, 0.0f, 8.0f);
		//glSamplerParameterf(m_deviceId, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_anisotropyAmount);
	}
	else
	{
		glSamplerParameterf(m_deviceId, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.0f);
	}
}

Sampler::~Sampler()
{
	glDeleteSamplers(1, &m_deviceId);
	m_deviceId = 0;
}
