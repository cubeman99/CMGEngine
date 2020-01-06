#include "cmgRenderParams.h"

RenderParams::RenderParams() :
	m_depthFunc(CompareFunction::k_less_equal),
	m_frontFace(FrontFace::k_clockwise),
	m_cullFace(CullFace::k_back),
	m_polygonMode(PolygonMode::k_fill),
	m_blendEnabled(false),
	m_depthTestEnabled(false),
	m_depthWriteEnabled(true),
	m_nearFarClippingEnabled(false),
	m_cullFaceEnabled(false),
	m_lineSmoothEnabled(false),
	m_polygonSmoothEnabled(false),
	m_clearColor(Color::BLACK),
	m_clearDepth(1.0f),
	m_clearStencil(0),
	m_clearBits(ClearBits::k_color | ClearBits::k_depth),
	m_blendFuncSource(BlendFunc::k_source_alpha),
	m_blendFuncDestination(BlendFunc::k_one_minus_source_alpha)
{
}
	
	
void RenderParams::SetClearBits(bool clearColor, bool clearDepth, bool clearStencil)
{
	m_clearBits = 0;
	if (clearColor)
		m_clearBits |= ClearBits::k_color;
	if (clearDepth)
		m_clearBits |= ClearBits::k_depth;
	if (clearStencil)
		m_clearBits |= ClearBits::k_stencil;
}

void RenderParams::SetBlendFunction(BlendFunc source, BlendFunc destination)
{
	m_blendFuncSource = source;
	m_blendFuncDestination = destination;
}
