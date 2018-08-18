#include "cmgSampler.h"
#include <cmgGraphics/cmgOpenGLIncludes.h>
#include <cmgGraphics/cmgTexture.h>
#include <cmgMath/cmgMathLib.h>

//-----------------------------------------------------------------------------
// OpenGL Enum Conversions
//-----------------------------------------------------------------------------
namespace
{
	GLenum TranslateTextureTarget(TextureTarget::value_type target)
	{
		switch (target)
		{
		case TextureTarget::TEXTURE_2D:
			return GL_TEXTURE_2D;
		case TextureTarget::TEXTURE_3D:
			return GL_TEXTURE_3D;
		case TextureTarget::TEXTURE_1D:
			return GL_TEXTURE_1D;
		case TextureTarget::TEXTURE_CUBE_MAP:
			return GL_TEXTURE_CUBE_MAP;
		}
		return GL_TEXTURE_2D;
	}

	GLenum TranslateTextureWrap(TextureWrap::value_type wrap)
	{
		switch (wrap)
		{
		case TextureWrap::CLAMP_TO_BORDER:
			return GL_CLAMP_TO_BORDER;
		case TextureWrap::CLAMP_TO_EDGE:
			return GL_CLAMP_TO_EDGE;
		case TextureWrap::REPEAT:
			return GL_REPEAT;
		case TextureWrap::MIRROR_REPEAT:
			return GL_MIRRORED_REPEAT;
		//case TextureWrap::MIRROR_CLAMP_TO_EDGE:
			//return GL_MIRROR_CLAMP_TO_EDGE; //TODO: Figure this out.
		}
		return GL_CLAMP_TO_BORDER;
	}

	GLenum TranslateMinMagFilter(TextureFilterOptions::value_type minMagFilter,
									TextureFilterOptions::value_type mipmapFilter)
	{
		//CMG_ASSERT(minMagFilter != TextureFilterOptions::NONE,
			//"You must specify a min and mag filter!");

		if (minMagFilter == TextureFilterOptions::LINEAR)
		{
			if (mipmapFilter == TextureFilterOptions::NONE)
				return GL_LINEAR;
			else if (mipmapFilter == TextureFilterOptions::LINEAR)
				return GL_LINEAR_MIPMAP_LINEAR;
			else if (mipmapFilter == TextureFilterOptions::NEAREST)
				return GL_LINEAR_MIPMAP_NEAREST;
		}
		else if (minMagFilter == TextureFilterOptions::NEAREST)
		{
			if (mipmapFilter == TextureFilterOptions::NONE)
				return GL_NEAREST;
			else if (mipmapFilter == TextureFilterOptions::LINEAR)
				return GL_NEAREST_MIPMAP_LINEAR;
			else if (mipmapFilter == TextureFilterOptions::NEAREST)
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
