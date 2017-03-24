#ifndef _CMG_RENDERER_H_
#define _CMG_RENDERER_H_

#include <cmgGraphics/render/cmgRenderParams.h>
#include <cmgMath/types/cmgMatrix4f.h>
#include <cmgMath/types/cmgVector3f.h>
#include <cmgMath/types/cmgQuaternion.h>
#include <cmgMath/types/cmgTransform3f.h>
//#include "Material.h"
//#include "Mesh.h"
#include <cmgGraphics/cmgShader.h>
#include <cmgGraphics/cmgVertexData.h>
//#include "ICamera.h"


class Renderer
{
public:
	Renderer();

	const RenderParams& GetRenderParams() const { return m_params; }
	
	void SetRenderParams(const RenderParams& renderParams) { m_params = renderParams; }

	void ApplyRenderSettings(bool clear = false);
	void Clear();

	//void SetModelMatrix(const Matrix4f& model);
	//void SetViewMatrix(const Matrix4f& view);
	//void SetProjectionMatrix(const Matrix4f& projection);
	//Matrix4f GetMVP();
	
	inline Shader* GetShader() { return m_activeShader; }

	void SetShader(Shader* shader);
	void SetDefaultShader(Shader* defaultShader);
	void SetViewMatrix(const Matrix4f& viewMatrix);
	void SetProjectionMatrix(const Matrix4f& projectionMatrix);

	void UpdateUniforms();

	//void RenderMesh(Mesh* mesh, Material* material, const Transform3f& transform);
	//void RenderMesh(Mesh* mesh, Material* material, const Matrix4f& modelMatrix);

private:
	RenderParams	m_params;
	Shader*			m_activeShader;
	Matrix4f		m_viewMatrix;
	Matrix4f		m_projectionMatrix;
	bool			m_isShaderChanged;


	static Shader* GetDefaultShader();
	static Shader* s_defaultShader;
};


#endif // _CMG_RENDERER_H_