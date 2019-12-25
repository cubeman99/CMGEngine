#ifndef _IMAGE_FORMAT_H_
#define _IMAGE_FORMAT_H_

#include <cmgGraphics/types/cmgColor.h>

// Image encoding formats
enum class ImageEncodingFormat
{
	UNKNOWN = -1,

	PNG,
	JPEG,
	BMP,
	TGA,
	DDS,
	
	COUNT,
	AUTO,
};
	
// Component-wise format for a pixel.
enum class PixelFormat
{
	UNKNOWN = -1,

	RED,
	GREEN,
	BLUE,

	RGB,
	BGR,
	RGBA,
	BGRA,
	
	LUMINANCE,		// Deprecated
	ALPHA,			// Deprecated
	LUMINANCE_ALPHA	// Deprecated
};

enum class PixelInternalFormat
{
	// Base Internal Formats
	RED,
	RG,
	RGB,
	RGBA,
	DEPTH_COMPONENT,
	STENCIL_INDEX,
	DEPTH_STENCIL,
			
	// TODO: Sized Internal Formats
};

// Formats for transfering pixel data.
enum class PixelTransferFormat
{
	// Transfers of normalized integer or floating-point color image data:
	RED,
	GREEN,
	BLUE,
	RG,
	RGB,
	BGR,
	RGBA,
	BGRA,

	// Transfers of non-normalized integer data:
	RED_INTEGER,
	GREEN_INTEGER,
	BLUE_INTEGER,
	RG_INTEGER,
	RGB_INTEGER,
	BGR_INTEGER,
	RGBA_INTEGER,
	BGRA_INTEGER,
			
	// Transfers of depth, stencil, or depth/stencil data:
	DEPTH_COMPONENT,	// Each element is a single depth value.
	STENCIL_INDEX,		// Each element is a single depth index.
	DEPTH_STENCIL,		// Each element is a pair of depth and stencil values
};

// Data type of the pixel data.
enum class PixelType
{
	TYPE_UNSIGNED_BYTE,
	TYPE_BYTE,
	TYPE_UNSIGNED_SHORT,
	TYPE_SHORT,
	TYPE_UNSIGNED_INT,
	TYPE_INT,
	TYPE_FLOAT,

	// TODO: Different bit storage formats.
	// Ex: TYPE_UNSIGNED_BYTE_3_3_2
};


#endif // _IMAGE_FORMAT_H_