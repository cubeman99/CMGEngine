#ifndef _CMG_GRAPHICS_SAMPLER_H_
#define _CMG_GRAPHICS_SAMPLER_H_

#include <cmgCore/cmgBase.h>
#include <cmgGraphics/cmgTexture.h>
#include <cmgGraphics/cmgTextureParams.h>


class Sampler
{
	friend class OpenGLRenderDevice;

public:
	Sampler(OpenGLRenderDevice& device, const TextureParams& params = TextureParams());
	~Sampler();


	inline uint32 GetId() const
	{
		return m_deviceId;
	}

private:
	OpenGLRenderDevice* m_device;
	uint32 m_deviceId;
};


#endif // _CMG_GRAPHICS_SAMPLER_H_