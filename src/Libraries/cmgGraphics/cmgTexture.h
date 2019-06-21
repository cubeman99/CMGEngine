#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <cmgGraphics/types/cmgColor.h>
#include <cmgGraphics/cmgTextureParams.h>
#include <cmgGraphics/cmgImageFormat.h>
#include <cmgCore/error/cmgError.h>
#include <cmgCore/io/cmgPath.h>

class RenderBuffer;


//-----------------------------------------------------------------------------
// OpenGL Texture
//-----------------------------------------------------------------------------

class Texture
{
	friend class Font;
	friend class RenderTarget;
	friend class OpenGLRenderDevice;
	friend class RenderBuffer;

public:
	typedef CubeMapFace::index_type cubemap_face_index;

public:
	Texture();
	Texture(const TextureParams& params);
	~Texture();

	// Texture Parameters
	const TextureParams& GetParams() const;
	void SetParams(const TextureParams& params);
	
	// Dimensions
	int GetWidth() const;
	int GetHeight() const;
	int GetDepth() const;

	// Mipmaps
	int GetNumMipMaps() const { return m_numMipMaps; }
	void GenerateMipMaps();

	// Anisotropy
	float GetAnisotropyAmount() const { return m_anisotropyAmount; }
		
	// Pixel read
	void ReadPixels(Color* pixels) const;
	void ReadPixels(PixelTransferFormat format, PixelType type, void* pixels) const;
	void ReadPixels(int mipmapLevel, Color* pixels) const;
	void ReadPixels(int mipmapLevel, PixelTransferFormat format, PixelType type, void* pixels) const;

	// Cubemap face write
	void WriteFacePixels(cubemap_face_index face, int width, int height, const Color* pixels);
	void WriteFacePixels(cubemap_face_index face, int width, int height, PixelTransferFormat format, PixelType type, const void* pixelData);
	void WriteFacePixelsAtLevel(cubemap_face_index face, int mipmapLevel, int width, int height, const Color* pixels);
	void WriteFacePixelsAtLevel(cubemap_face_index face, int mipmapLevel, int width, int height, PixelTransferFormat format, PixelType type, const void* pixelData);
		
	// 1D texture write
	void WritePixels1D(int width, const Color* pixels);
	void WritePixels1D(int width, PixelTransferFormat format, PixelType type, const void* pixelData);
	void WritePixelsAtLevel1D(int mipmapLevel, int width, const Color* pixels);
	void WritePixelsAtLevel1D(int mipmapLevel, int width, PixelTransferFormat format, PixelType type, const void* pixelData);
		
	// 2D texture write
	void WritePixels2D(int width, int height, const Color* pixels);
	void WritePixels2D(int width, int height, PixelTransferFormat format, PixelType type, const void* pixelData);
	void WritePixelsAtLevel2D(int mipmapLevel, int width, int height, const Color* pixels);
	void WritePixelsAtLevel2D(int mipmapLevel, int width, int height, PixelTransferFormat format, PixelType type, const void* pixelData);
		
	// 3D texture write
	void WritePixels3D(int width, int height, int depth, const Color* pixels);
	void WritePixels3D(int width, int height, int depth, PixelTransferFormat format, PixelType type, const void* pixelData);
	void WritePixelsAtLevel3D(int mipmapLevel, int width, int height, int depth, const Color* pixels);
	void WritePixelsAtLevel3D(int mipmapLevel, int width, int height, int depth, PixelTransferFormat format, PixelType type, const void* pixelData);

	void InitRenderTarget();

	static Texture* LoadTexture(const std::string& fileName, const TextureParams& params = TextureParams());
	static Error LoadTexture(Texture*& outTexture, const std::string& fileName, const TextureParams& params = TextureParams());
	static Error SaveTexture(Texture* texture, const Path& path);


	inline unsigned int GetGLTextureID() const { return m_glTextureId; }

private:
	void DoBindTexture();
	void DoUnbindTexture();
	unsigned int GetGLTextureTarget() const;

	// Prevent copying textures.
	Texture(const Texture& other) {}
	void operator=(const Texture& other) {}

	int32 m_width;				// The width of the texture, in texels.
	int32 m_height;				// The height of the texture, in texels.
	int32 m_depth;
	TextureParams m_params;
	PixelFormat m_format;
	int32 m_numMipMaps;
	float m_anisotropyAmount;
	uint32 m_glTextureId;		// The opengl texture handle.
	uint32 m_glFrameBufferId;	// The frame buffer (for render targets).
	uint32 m_glDepthBufferId;	// The depth buffer (for render targets).
};


#endif // _TEXTURE_H_