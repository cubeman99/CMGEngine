#ifndef _PHYSICS_COLLISION_TEST_APP_H_
#define _PHYSICS_COLLISION_TEST_APP_H_

#include <cmgApplication/cmg_application.h>
#include <cmgMath/cmg_math.h>
#include <cmgPhysics/cmg_physics.h>
#include <cmgPhysics/cmgQuickHull.h>


class CollisionTestApp : public Application
{
public:
	CollisionTestApp();
	
	void OnInitialize() override;
	void OnQuit() override;
	void OnResizeWindow(int width, int height) override;
	void OnUpdate(float timeDelta) override;
	void OnRender() override;

	void Reset();

	void UpdateDebugBodyControls(RigidBody* body, float timeDelta);


	void CreateMinkowskiDifferencePointCloud(std::vector<Vector3f>& points, Collider* colliderA, Collider* colliderB);
	void DrawGrid();
	void DrawSimplex(const Simplex& simplex);
	void DrawContactPoint(const Contact& contact);
	void DrawRigidBody(RigidBody* body);

	void DrawQuickHull(QuickHull* hull);

private:
	
	Transform3f m_cameraTransform;
	Matrix4f m_cameraProjection;

	CollisionSphere	m_physSphere;
	CollisionBox	m_physCube;
	CollisionBox	m_physGround;

	DebugDraw*		m_debugDraw;
	RigidBody*		m_testBody1;
	RigidBody*		m_testBody2;

	bool			m_showMinkowskiDifference;

	EPAResult m_epa;
	QuickHull	m_quickHull;
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

	bool m_showContacts;

	Shader*		m_shader;
	SpriteFont*	m_font;
	Mesh*		m_meshSphere;
	Mesh*		m_meshCube;
	
	Timer m_frameTimer;

	PhysicsEngine m_physicsEngine;
};


#endif // _PHYSICS_COLLISION_TEST_APP_H_