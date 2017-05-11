#ifndef _DEBUG_DRAW_H_
#define _DEBUG_DRAW_H_

#include <cmgGraphics/cmg_graphics.h>
#include <cmgPhysics/colliders/cmgSphereCollider.h>
#include <cmgPhysics/colliders/cmgBoxCollider.h>

class DebugDraw
{
public:
	DebugDraw();
	~DebugDraw();

	void Begin();

	// Render options
	void SetLineWidth(float lineWidth);
	void SetPointSize(float pointSize);
	void SetShaded(bool shaded, const Vector3f& lightDirection = Vector3f(-1, -2, -3));
	void SetViewProjection(const Matrix4f& viewProjection);

	// Colliders
	void DrawWireCollider(const Collider* collider, const Color& color);
	void DrawFilledCollider(const Collider* collider, const Color& color);
	void DrawWireBoxCollider(const BoxCollider* box, const Color& color);
	void DrawFilledBoxCollider(const BoxCollider* box, const Color& color);
	void DrawWireSphereCollider(const SphereCollider* sphere, const Color& color);
	void DrawFilledSphereCollider(const SphereCollider* sphere, const Color& color);

	// Primitives
	void DrawWireBox(const Matrix4f& modelMatrix, const Vector3f& halfSize, const Color& color);
	void DrawFilledBox(const Matrix4f& modelMatrix, const Vector3f& halfSize, const Color& color);
	void DrawWireSphere(const Matrix4f& modelMatrix, float radius, const Color& color);
	void DrawFilledSphere(const Matrix4f& modelMatrix, float radius, const Color& color);
	void DrawWireCylinder(const Matrix4f& modelMatrix, float radius, float halfHeight, const Color& color);
	void DrawFilledCylinder(const Matrix4f& modelMatrix, float radius, float halfHeight, const Color& color);
	void DrawWireCylinder(const Matrix4f& modelMatrix, const Vector3f& a, const Vector3f& b, float radius, const Color& color);
	void DrawFilledCylinder(const Matrix4f& modelMatrix, const Vector3f& a, const Vector3f& b, float radius, const Color& color);


private:
	void BeginDrawWire();
	void BeginDrawFill();
	void DrawMesh(Mesh* mesh, const Matrix4f& modelMatrix, const Color& color);
	
	// Graphics options
	Matrix4f m_viewProjection;
	float m_lineWidth;
	float m_pointSize;
	bool m_shaded;
	Vector3f m_lightDirection;

	// Shaders
	Shader* m_shaderSolidColor;
	Shader* m_shaderShadedColor;

	// Meshes
	Mesh* m_meshCube;
	Mesh* m_meshSphere;
	Mesh* m_meshSphereLowRes;
	Mesh* m_meshCylinder;
};


#endif // _DEBUG_DRAW_H_