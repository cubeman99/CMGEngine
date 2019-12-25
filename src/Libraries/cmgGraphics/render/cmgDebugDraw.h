#ifndef _CMG_GRAPHICS_DEBUG_DRAW_H_
#define _CMG_GRAPHICS_DEBUG_DRAW_H_

#include <cmgGraphics/cmg_graphics.h>
#include <cmgPhysics/cmg_physics.h>


class DebugDraw
{
public:
	DebugDraw();
	~DebugDraw();

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
	void DrawWireCylinderCollider(const CylinderCollider* cylinder, const Color& color);
	void DrawFilledCylinderCollider(const CylinderCollider* cylinder, const Color& color);
	void DrawWireCapsuleCollider(const CapsuleCollider* capsule, const Color& color);
	void DrawFilledCapsuleCollider(const CapsuleCollider* capsule, const Color& color);
	void DrawWireConeCollider(const ConeCollider* cone, const Color& color);
	void DrawFilledConeCollider(const ConeCollider* cone, const Color& color);
	void DrawWirePolygonCollider(const PolygonCollider* poly, const Color& color);
	void DrawFilledPolygonCollider(const PolygonCollider* poly, const Color& color);
	void DrawDrawConvexMeshCollider(const ConvexMeshCollider* mesh, const Color& color);
	void DrawFilledConvexMeshCollider(const ConvexMeshCollider* mesh, const Color& color);

	// Primitives
	void DrawWireBox(const Matrix4f& modelMatrix, const Vector3f& halfSize, const Color& color);
	void DrawFilledBox(const Matrix4f& modelMatrix, const Vector3f& halfSize, const Color& color);
	void DrawWireSphere(const Matrix4f& modelMatrix, float radius, const Color& color);
	void DrawFilledSphere(const Matrix4f& modelMatrix, float radius, const Color& color);
	void DrawWireCylinder(const Matrix4f& modelMatrix, float radius, float halfHeight, const Color& color);
	void DrawFilledCylinder(const Matrix4f& modelMatrix, float radius, float halfHeight, const Color& color);
	void DrawWireCylinder(const Matrix4f& modelMatrix, const Vector3f& a, const Vector3f& b, float radius, const Color& color);
	void DrawFilledCylinder(const Matrix4f& modelMatrix, const Vector3f& a, const Vector3f& b, float radius, const Color& color);
	void DrawWireCapsule(const Matrix4f& modelMatrix, float radius, float halfHeight, const Color& color);
	void DrawFilledCapsule(const Matrix4f& modelMatrix, float radius, float halfHeight, const Color& color);
	void DrawWireCapsule(const Matrix4f& modelMatrix, const Vector3f& a, const Vector3f& b, float radius, const Color& color);
	void DrawFilledCapsule(const Matrix4f& modelMatrix, const Vector3f& a, const Vector3f& b, float radius, const Color& color);
	void DrawWireCone(const Matrix4f& modelMatrix, float radius, float height, const Color& color);
	void DrawFilledCone(const Matrix4f& modelMatrix, float radius, float height, const Color& color);

	// Simple Shapes
	void DrawLine(const Matrix4f& modelMatrix, const Vector3f& p1, const Vector3f& p2, const Color& color, float width = 1.0f);
	void DrawPoint(const Matrix4f& modelMatrix, const Vector3f& point, const Color& color, float size = 1.0f);

	void DrawGrid(const Matrix4f& modelMatrix, const Vector3f& center,
		float squareSize, uint32 majorCount, float minorLineWidth,
		float majorLineWidth, const Color& minorLineColor,
		const Color& majorLineColor, float gridSize);

	Color GetShadedColor(const Vector3f& normal, const Color& color);

	void BeginImmediate(const Matrix4f& transform = Matrix4f::IDENTITY);
	
	void DrawMesh(Mesh* mesh, const Matrix4f& modelMatrix, const Color& color);

private:
	void BeginDrawWire();
	void BeginDrawFill();
	
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
	Mesh* m_meshCone;
};


#endif // _CMG_GRAPHICS_DEBUG_DRAW_H_