#ifndef _PHYSICS_APP_H_
#define _PHYSICS_APP_H_

#include <cmgApplication/cmg_application.h>
#include <cmgMath/cmg_math.h>
#include <cmgPhysics/cmg_physics.h>
#include "PrimitiveMeshes.h"
#include <driving/SpriteFont.h>
#include <graphics/DebugDraw.h>


class PhysObject
{
public:
	RigidBody* m_rigidBody;
};


class PhysicsApp : public Application
{
public:
	PhysicsApp();
	
	void OnInitialize() override;
	void OnQuit() override;
	void OnResizeWindow(int width, int height) override;
	void OnUpdate(float timeDelta) override;
	void OnRender() override;

	void UpdateDebugBodyControls(RigidBody* body, float timeDelta);

	void DrawSphere(float radius, const Matrix4f& transform, const Color& color);
	void DrawOutlinedBox(const Vector3f& halfSize, const Matrix4f& transform, const Color& color);
	void DrawBox(const Vector3f& halfSize, const Matrix4f& transform, const Color& color);
	void DrawBoxFace(const Vector3f& halfSize, const Matrix4f& transform, unsigned int faceIndex, const Color& color);
	void DrawBoxEdge(const CollisionBox& box, unsigned int vertexIndex1, unsigned int vertexIndex2, const Color& color);

	void DrawContactPoint(const Vector3f& contactPoint, const Vector3f& contactNormal);
	void DrawContactEdge(const Vector3f& endPoint1, const Vector3f& endPoint2);

	void DrawDebugBoxInfo(const CollisionBox& box);

	void DrawSATViewGrid(
		CollisionBox* first,
		CollisionBox* second);
	void DrawSATViewPort(
		CollisionBox* one,
		CollisionBox* two,
		unsigned int axisIndex,
		const Vector2f& viewPortTopLeft,
		const Vector2f& viewPortSize);
	
	void DrawRigidBody(RigidBody* body);
	void DrawPrimitive(CollisionPrimitive* primitive, const Color& color, const Matrix4f& modelMatrix);
	void DrawMesh(Mesh* mesh, const Color& color, const Matrix4f& modelMatrix);

private:
	
	Transform3f m_cameraTransform;
	Matrix4f m_cameraProjection;

	CollisionSphere	m_physSphere;
	CollisionBox	m_physCube;
	CollisionBox	m_physGround;

	DebugDraw*		m_debugDraw;
	RigidBody*		m_testBody1;
	RigidBody*		m_testBody2;

	EPAResult m_epa;

	struct 
	{
		SupportPoint nextPoint;
		Simplex simplex;
		Vector3f direction;
		unsigned int iteration;
		Collider* shapeA;
		Collider* shapeB;
		bool done;
		bool result;
	} m_gjk;

	RenderParams m_renderParams;

	bool m_simulationPaused;
	float m_simulationSpeed;

	std::vector<PhysObject*> m_physObjects;

	Shader*		m_shader;
	SpriteFont*	m_font;
	Mesh*		m_meshSphere;
	Mesh*		m_meshCube;
	
	Timer m_frameTimer;

	PhysicsEngine m_physicsEngine;
};


#endif // _PHYSICS_APP_H_