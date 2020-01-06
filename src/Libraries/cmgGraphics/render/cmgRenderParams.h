#ifndef _CMG_GRAPHICS_RENDER_PARAMS_H_
#define _CMG_GRAPHICS_RENDER_PARAMS_H_

#include <cmgGraphics/types/cmgColor.h>


//-----------------------------------------------------------------------------
// Compare Function (used for depth function)
//-----------------------------------------------------------------------------

enum class CompareFunction
{
	k_always_pass,
	k_always_fail,
	k_less,
	k_less_equal,
	k_greater,
	k_greater_equal,
	k_equal,
	k_not_equal,
};

//-----------------------------------------------------------------------------
// Blend Function
//-----------------------------------------------------------------------------

enum class BlendFunc
{
	k_zero,
	k_one,
	k_source_color,
	k_one_minus_source_color,
	k_destination_color,
	k_one_minus_destination_color,
	k_source_alpha,
	k_one_minus_source_alpha,
	k_destination_alpha,
	k_one_minus_destination_alpha,
	k_constant_color,
	k_one_minus_constant_color,
	k_constant_alpha,
	k_one_minus_constant_alpha,
	k_source_alpha_saturate,
	k_source1_color,
	k_one_minus_source1_color,
	k_source1_alpha,
	k_one_minus_source1_alpha,
};


//-----------------------------------------------------------------------------
// Clear Bits
//-----------------------------------------------------------------------------

struct ClearBits
{
	typedef int value_type;

	enum
	{
		k_color		= 0x1,
		k_depth		= 0x2,
		k_stencil	= 0x4,
	};
};

//-----------------------------------------------------------------------------
// Culling
//-----------------------------------------------------------------------------

// Winding order that defines the front of a face.
enum class FrontFace
{
	k_clockwise,
	k_counter_clockwise,
};

// Which side(s) of a face is culled.
enum class CullFace
{
	k_front,
	k_back,
	k_front_and_back,
};

//-----------------------------------------------------------------------------
// Polygon Mode
//-----------------------------------------------------------------------------

// Method for rasterizing polygon shapes.
enum class PolygonMode
{
	k_point,
	k_line,
	k_fill,
};


//-----------------------------------------------------------------------------
// Render Params
//-----------------------------------------------------------------------------

class RenderParams
{
public:
	RenderParams();

	//-----------------------------------------------------------------------------
	// Getters

	CompareFunction GetDepthFunction() const { return m_depthFunc; }
	FrontFace GetFrontFace() const { return m_frontFace; }
	CullFace GetCullFace() const { return m_cullFace; }
	PolygonMode GetPolygonMode() const { return m_polygonMode; }
	ClearBits::value_type GetClearBits() const { return m_clearBits; }
	BlendFunc GetBlendFunctionSource() const { return m_blendFuncSource; }
	BlendFunc GetBlendFunctionDestination() const { return m_blendFuncDestination; }
	const Color& GetClearColor() const { return m_clearColor; }
	float GetClearDepth() const { return m_clearDepth; }
	int32 GetClearStencil() const { return m_clearStencil; }
		
	bool IsBlendEnabled() const { return m_blendEnabled; }
	bool IsDepthTestEnabled() const { return m_depthTestEnabled; }
	bool IsDepthBufferWriteEnabled() const { return m_depthWriteEnabled; }
	bool IsNearFarPlaneClippingEnabled() const { return m_nearFarClippingEnabled; }
	bool IsCullFaceEnabled() const { return m_cullFaceEnabled; }
	bool IsLineSmoothEnabled() const { return m_lineSmoothEnabled; }
	bool IsPolygonSmoothEnabled() const { return m_polygonSmoothEnabled; }
		
	//-----------------------------------------------------------------------------
	// Setters
		
	void SetDepthFunction(CompareFunction depthFunc) { m_depthFunc = depthFunc; }
	void SetFrontFace(FrontFace frontFace) { m_frontFace = frontFace; }
	void SetCullFace(CullFace cullFace) { m_cullFace = cullFace; }
	void SetPolygonMode(PolygonMode polygonMode) { m_polygonMode = polygonMode; }
	void SetClearBits(ClearBits::value_type clearBits) { m_clearBits = clearBits; }
	void SetClearBits(bool clearColor, bool clearDepth, bool clearStencil);
	void SetClearColor(const Color& clearColor) { m_clearColor = clearColor; }
	void SetClearDepth(float clearDepth) { m_clearDepth = clearDepth; }
	void SetClearStencil(int32 clearStencil) { m_clearStencil = clearStencil; }
		
	void EnableBlend(bool enabled) { m_blendEnabled = enabled; }
	void EnableDepthTest(bool enabled) { m_depthTestEnabled = enabled; }
	void EnableDepthBufferWrite(bool enabled) { m_depthWriteEnabled = enabled; }
	void EnableNearFarPlaneClipping(bool enabled) { m_nearFarClippingEnabled = enabled; }
	void EnableCullFace(bool enabled) { m_cullFaceEnabled = enabled; }
	void EnableLineSmooth(bool enabled) { m_lineSmoothEnabled = enabled; }
	void EnablePolygonSmooth(bool enabled) { m_polygonSmoothEnabled = enabled; }

	void SetBlendFunction(BlendFunc source, BlendFunc destination);

		
private:
	CompareFunction m_depthFunc;
	FrontFace m_frontFace;
	CullFace m_cullFace;
	PolygonMode m_polygonMode;
	BlendFunc m_blendFuncSource;
	BlendFunc m_blendFuncDestination;
	ClearBits::value_type m_clearBits;
	Color m_clearColor;
	float m_clearDepth;
	int32 m_clearStencil;
	bool m_blendEnabled;
	bool m_depthTestEnabled;
	bool m_depthWriteEnabled;
	bool m_nearFarClippingEnabled;
	bool m_cullFaceEnabled;
	bool m_lineSmoothEnabled;
	bool m_polygonSmoothEnabled;
	
	// TODO: for RenderParams:
	//  * m_fbo (Frame buffer object)
	//  * m_pointSize (for the points polygon mode)
	//  * m_dimensions (dimensions of the frame buffer)
};


#endif // _CMG_GRAPHICS_RENDER_PARAMS_H_