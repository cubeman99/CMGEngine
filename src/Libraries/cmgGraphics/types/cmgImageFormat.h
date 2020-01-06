#ifndef _IMAGE_FORMAT_H_
#define _IMAGE_FORMAT_H_

#include <cmgGraphics/types/cmgColor.h>

// Image encoding formats
enum class ImageEncodingFormat
{
	k_unknown = -1,
	k_png,
	k_jpeg,
	k_bmp,
	k_tga,
	k_dds,
	k_count,
	k_auto,
};
	
// Component-wise format for a pixel.
enum class PixelFormat
{
	k_unknown = -1,
	k_red,
	k_green,
	k_blue,
	k_rgb,
	k_bgr,
	k_rgba,
	k_bgra,
	k_luminance,		// deprecated
	k_alpha,			// deprecated
	k_luminance_alpha	// deprecated
};

enum class PixelInternalFormat
{
	// Base Internal Formats
	k_red,
	k_rg,
	k_rgb,
	k_rgba,
	k_depth_component,
	k_stencil_index,
	k_depth_stencil,
			
	// Sized Internal Formats
	k_rgba_16f,
	k_rgba_32f,
	k_depth_16,
	k_depth_24,
	k_depth_32,
	k_depth_32f,
	k_depth24_stencil8,
};

// Formats for transfering pixel data.
enum class PixelTransferFormat
{
	// Transfers of normalized integer or floating-point color image data:
	k_red,
	k_green,
	k_blue,
	k_rg,
	k_rgb,
	k_bgr,
	k_rgba,
	k_bgra,

	// Transfers of non-normalized integer data:
	k_red_integer,
	k_green_integer,
	k_blue_integer,
	k_rg_integer,
	k_rgb_integer,
	k_bgr_integer,
	k_rgba_integer,
	k_bgra_integer,
			
	// Transfers of depth, stencil, or depth/stencil data:
	k_depth_component,	// Each element is a single depth value
	k_stencil_index,	// Each element is a single depth index
	k_depth_stencil,	// Each element is a pair of depth and stencil values
};

// Data type of the pixel data.
enum class PixelType
{
	k_unsigned_byte,
	k_byte,
	k_unsigned_short,
	k_short,
	k_unsigned_int,
	k_int,
	k_float,

	// TODO: Different bit storage formats.
	// Ex: TYPE_UNSIGNED_BYTE_3_3_2
};


#endif // _IMAGE_FORMAT_H_