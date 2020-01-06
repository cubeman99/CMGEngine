#ifndef _TEXTURE_PARAMS_H_
#define _TEXTURE_PARAMS_H_

#include <cmgGraphics/types/cmgColor.h>
#include <cmgGraphics/types/cmgImageFormat.h>


//-----------------------------------------------------------------------------
// The types of targets that a texture can be bound to
enum class TextureTarget
{
	k_texture_1d,
	k_texture_2d,
	k_texture_3d,
	k_texture_cube_map,
	k_num_texture_targets
};
	
//-----------------------------------------------------------------------------
// Modes for wrapping texture coordinates
enum class TextureWrap
{
	// The coordinate will be clamped between 0.0 and 1.0.
	k_clamp_to_edge,

	// The coordinates that fall outside the range will be given a specified
	// border color.
	k_clamp_to_border,

	// The texture will also be repeated, but it will be mirrored when the
	// integer part of the coordinate is odd.
	k_mirror_repeat,

	// The integer part of the coordinate will be ignored and a repeating
	// pattern is formed.
	k_repeat,
};
	
//-----------------------------------------------------------------------------
// Modes for texture filtering

// Texture filtering presets, each specifies the min, mag, and mipmap filters.
enum class TextureFilter
{
	k_nearest,		// min = NEAREST, mag = NEAREST, mipmap = NONE.
	k_bilinear,		// min = LINEAR,  mag = LINEAR,  mipmap = NEAREST.
	k_trilinear,	// min = LINEAR,  mag = LINEAR,  mipmap = LINEAR.
	k_anisotropic,	// Anisotropic filtering on top of trilinear filtering.
};

// Texture filtering modes
enum class TextureFilterOptions
{
	k_none,		// No filtering (used to disable mipmap selection).
	k_nearest,	// Sample nearest texels.
	k_linear,	// Linear interpolation with neighboring texels.
};
	
//-----------------------------------------------------------------------------
// Indices for cubemap faces
struct CubeMapFace
{
	typedef int index_type;

	enum
	{
		k_positive_x = 0,
		k_negative_x,
		k_positive_y,
		k_negative_y,
		k_positive_z,
		k_negative_z,
		k_num_cubemap_faces,
	};
};


//-----------------------------------------------------------------------------
// Texture Parameters
//-----------------------------------------------------------------------------

class TextureParams
{
public:
	TextureParams();
	TextureParams(TextureTarget target);

	//-----------------------------------------------------------------------------
	// Getters

	TextureTarget GetTarget() const { return m_target; }
	TextureFilterOptions GetMinFilter() const { return m_minFilter; }
	TextureFilterOptions GetMagFilter() const { return m_magFilter; }
	TextureFilterOptions GetMipMapFilter() const { return m_mipFilter; }
	TextureWrap GetWrapS() const { return m_wrapS; }
	TextureWrap GetWrapT() const { return m_wrapT; }
	TextureWrap GetWrapR() const { return m_wrapR; }
	PixelInternalFormat GetInternalFormat() const { return m_internalFormat; }
	bool AutoGenMipMaps() const { return m_autoGenMipMaps; }
	bool UseAnisotropy() const { return m_anisotropy; }

	//-----------------------------------------------------------------------------
	// Setters

	void SetTarget(TextureTarget target) { m_target = target; }
	void SetMinFilter(TextureFilterOptions minFilter) { m_minFilter = minFilter; }
	void SetMagFilter(TextureFilterOptions magFilter) { m_magFilter = magFilter; }
	void SetMipMapFilter(TextureFilterOptions mipMapFilter) { m_mipFilter = mipMapFilter; }
	void SetWrapS(TextureWrap wrapS) { m_wrapS = wrapS; }
	void SetWrapT(TextureWrap wrapT) { m_wrapT = wrapT; }
	void SetWrapR(TextureWrap wrapR) { m_wrapR = wrapR; }
	void SetInternalFormat(PixelInternalFormat internalFormat) { m_internalFormat = internalFormat; }
	void SetAutoGenMipMaps(bool autoGenMipMaps) { m_autoGenMipMaps = autoGenMipMaps; }
	void SetAnisotropy(bool anisotropy) { m_anisotropy = anisotropy; }

	void SetFiltering(TextureFilterOptions minFilter,
		TextureFilterOptions magFilter,
		TextureFilterOptions mipMapFilter);

	void SetFiltering(TextureFilter filtering);

	void SetWrap(TextureWrap wrap);

private:
	TextureTarget m_target;
	TextureFilterOptions m_minFilter; // Minimize filter
	TextureFilterOptions m_magFilter; // Magnify filter
	TextureFilterOptions m_mipFilter; // Mipmap filter
	TextureWrap m_wrapS;
	TextureWrap m_wrapT;
	TextureWrap m_wrapR;

	PixelInternalFormat m_internalFormat;
	bool m_autoGenMipMaps; // Automatically generate mipmaps
	bool m_anisotropy;
};


#endif // _TEXTURE_PARAMS_H_