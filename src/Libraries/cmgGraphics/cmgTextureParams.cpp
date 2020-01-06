#include "cmgTextureParams.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TextureParams::TextureParams() :
	TextureParams(TextureTarget::k_texture_2d)
{
}

TextureParams::TextureParams(TextureTarget target) :
	m_target(target),
	m_minFilter(TextureFilterOptions::k_linear), // Default is trilinear filtering
	m_magFilter(TextureFilterOptions::k_linear),
	m_mipFilter(TextureFilterOptions::k_none),
	m_anisotropy(false),
	m_wrapS(TextureWrap::k_clamp_to_edge),
	m_wrapT(TextureWrap::k_clamp_to_edge),
	m_wrapR(TextureWrap::k_clamp_to_edge),
	m_internalFormat(PixelInternalFormat::k_rgba),
	m_autoGenMipMaps(true)
{
}
	
//-----------------------------------------------------------------------------
// Mutators
//-----------------------------------------------------------------------------

void TextureParams::SetFiltering(TextureFilterOptions minFilter,
	TextureFilterOptions magFilter,
	TextureFilterOptions mipMapFilter)
{
	m_minFilter = minFilter;
	m_magFilter = magFilter;
	m_mipFilter = mipMapFilter;
}

void TextureParams::SetFiltering(TextureFilter filtering)
{
	if (filtering == TextureFilter::k_nearest)
	{
		m_minFilter = TextureFilterOptions::k_nearest;
		m_magFilter = TextureFilterOptions::k_nearest;
		m_mipFilter = TextureFilterOptions::k_none;
		m_anisotropy = false;
	}
	else if (filtering == TextureFilter::k_bilinear)
	{
		m_minFilter = TextureFilterOptions::k_linear;
		m_magFilter = TextureFilterOptions::k_linear;
		m_mipFilter = TextureFilterOptions::k_nearest;
		m_anisotropy = false;
	}
	else if (filtering == TextureFilter::k_trilinear)
	{
		m_minFilter = TextureFilterOptions::k_linear;
		m_magFilter = TextureFilterOptions::k_linear;
		m_mipFilter = TextureFilterOptions::k_linear;
		m_anisotropy = false;
	}
	else if (filtering == TextureFilter::k_anisotropic)
	{
		m_minFilter = TextureFilterOptions::k_linear;
		m_magFilter = TextureFilterOptions::k_linear;
		m_mipFilter = TextureFilterOptions::k_linear;
		m_anisotropy = true;
	}
}

void TextureParams::SetWrap(TextureWrap wrap)
{
	m_wrapS = wrap;
	m_wrapT = wrap;
	m_wrapR = wrap;
}
