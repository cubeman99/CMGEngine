#ifndef _RENDER_PARAMS_H_
#define _RENDER_PARAMS_H_

#include <cmgGraphics/types/cmgColor.h>


//-----------------------------------------------------------------------------
// Compare Function (used for depth function)
//-----------------------------------------------------------------------------

struct CompareFunction
{
	typedef int value_type;

	enum
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
};

//-----------------------------------------------------------------------------
// Blend Function
//-----------------------------------------------------------------------------

struct BlendFunc
{
	typedef int value_type;
		
	enum
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

	BlendFunc(value_type source, value_type destination)
		: source(source)
		, destination(destination)
	{ }

	value_type source;
	value_type destination;
};

//-----------------------------------------------------------------------------
// Clear Bits
//-----------------------------------------------------------------------------

struct ClearBits
{
	typedef int value_type;

	enum
	{
		k_color_buffer_bit		= 0x1,
		k_depth_buffer_bit		= 0x2,
		k_stencil_buffer_bit	= 0x4,
	};
};

//-----------------------------------------------------------------------------
// Culling
//-----------------------------------------------------------------------------

// Winding order that defines the front of a face.
struct FrontFace
{
	typedef int value_type;
		
	enum
	{
		k_clockwise,
		k_counter_clockwise,
	};
};

// Which side(s) of a face is culled.
struct CullFace
{
	typedef int value_type;
		
	enum
	{
		k_front,
		k_back,
		k_front_and_back,
	};
};

//-----------------------------------------------------------------------------
// Polygon Mode
//-----------------------------------------------------------------------------

// Method for rasterizing polygon shapes.
struct PolygonMode
{
	typedef int value_type;
		
	enum
	{
		k_point,
		k_line,
		k_fill,
	};
};


//-----------------------------------------------------------------------------
// Render Params
//-----------------------------------------------------------------------------

class RenderParams
{
public:
	RenderParams();

	//-----------------------------------------------------------------------------
	// Getters.

	CompareFunction::value_type	GetDepthFunction()	const { return m_depthFunc; }
	FrontFace::value_type		GetFrontFace()		const { return m_frontFace; }
	CullFace::value_type		GetCullFace()		const { return m_cullFace; }
	PolygonMode::value_type		GetPolygonMode()	const { return m_polygonMode; }
	ClearBits::value_type		GetClearBits()		const { return m_clearBits; }
	const BlendFunc&			GetBlendFunction()	const { return m_blendFunc; }
	const Color&				GetClearColor()		const { return m_clearColor; }
		
	bool IsBlendEnabled()					const {	return m_blendEnabled; }
	bool IsDepthTestEnabled()				const { return m_depthTestEnabled; }
	bool IsDepthBufferWriteEnabled()		const {	return m_depthWriteEnabled; }
	bool IsNearFarPlaneClippingEnabled()	const {	return m_nearFarClippingEnabled; }
	bool IsCullFaceEnabled()				const {	return m_cullFaceEnabled; }
	bool IsLineSmoothEnabled()				const {	return m_lineSmoothEnabled; }
	bool IsPolygonSmoothEnabled()			const { return m_polygonSmoothEnabled; }
		
	//-----------------------------------------------------------------------------
	// Setters.
		
	void SetDepthFunction(CompareFunction::value_type depthFunc)	{ m_depthFunc = depthFunc; }
	void SetFrontFace(FrontFace::value_type frontFace)				{ m_frontFace = frontFace; }
	void SetCullFace(CullFace::value_type cullFace)					{ m_cullFace = cullFace; }
	void SetPolygonMode(PolygonMode::value_type polygonMode)		{ m_polygonMode = polygonMode; }
	void SetClearBits(ClearBits::value_type clearBits)				{ m_clearBits = clearBits; }
	void SetClearColor(const Color& clearColor)						{ m_clearColor = clearColor; }
		
	void EnableBlend(bool enabled)					{ m_blendEnabled = enabled; }
	void EnableDepthTest(bool enabled)				{ m_depthTestEnabled = enabled; }
	void EnableDepthBufferWrite(bool enabled)		{ m_depthWriteEnabled = enabled; }
	void EnableNearFarPlaneClipping(bool enabled)	{ m_nearFarClippingEnabled = enabled; }
	void EnableCullFace(bool enabled)				{ m_cullFaceEnabled = enabled; }
	void EnableLineSmooth(bool enabled)				{ m_lineSmoothEnabled = enabled; }
	void EnablePolygonSmooth(bool enabled)			{ m_polygonSmoothEnabled = enabled; }

	void SetBlendFunction(BlendFunc::value_type source, BlendFunc::value_type destination);

		
private:
	CompareFunction::value_type	m_depthFunc;
	FrontFace::value_type		m_frontFace;
	CullFace::value_type		m_cullFace;
	PolygonMode::value_type		m_polygonMode;
	BlendFunc					m_blendFunc;
	ClearBits::value_type		m_clearBits;
	Color						m_clearColor;
	bool						m_blendEnabled;
	bool						m_depthTestEnabled;
	bool						m_depthWriteEnabled;
	bool						m_nearFarClippingEnabled;
	bool						m_cullFaceEnabled;
	bool						m_lineSmoothEnabled;
	bool						m_polygonSmoothEnabled;
	
	// TODO: for RenderParams:
	//  * m_fbo (Frame buffer object)
	//  * m_pointSize (for the points polygon mode)
	//  * m_dimensions (dimensions of the frame buffer)
};


#endif // _RENDER_PARAMS_H_