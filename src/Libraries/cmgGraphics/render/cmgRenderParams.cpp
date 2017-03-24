#include "cmgRenderParams.h"

RenderParams::RenderParams()
	: m_depthFunc(CompareFunction::k_less_equal)
	, m_frontFace(FrontFace::k_clockwise)
	, m_cullFace(CullFace::k_back)
	, m_polygonMode(PolygonMode::k_fill)
	, m_blendEnabled(false)
	, m_depthTestEnabled(false)
	, m_depthWriteEnabled(true)
	, m_nearFarClippingEnabled(false)
	, m_cullFaceEnabled(false)
	, m_lineSmoothEnabled(false)
	, m_polygonSmoothEnabled(false)
	, m_clearColor(Color::BLACK)
	, m_clearBits(ClearBits::k_color_buffer_bit | ClearBits::k_depth_buffer_bit)
	, m_blendFunc(BlendFunc::k_source_alpha, BlendFunc::k_one_minus_source_alpha)
{
}
	
	
void RenderParams::SetBlendFunction(BlendFunc::value_type source,
									BlendFunc::value_type destination)
{
	m_blendFunc.source = source;
	m_blendFunc.destination = destination;
}
