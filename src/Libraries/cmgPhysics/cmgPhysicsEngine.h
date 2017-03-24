#ifndef _CMG_PHYSICS_ENGINE_H_
#define _CMG_PHYSICS_ENGINE_H_

#include <cmgMath/types/cmgVector4f.h>
#include <cmgPhysics/cmgContact.h>
#include <cmgPhysics/cmgRigidBody.h>
#include <cmgPhysics/cmgCollisionDetector.h>


//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------
class PhysicsEngine
{
public:
	PhysicsEngine();
	~PhysicsEngine();
	
	void Initialize();
	void Simulate(float timeDelta);
	void AddBody(RigidBody* body);
	void ClearBodies();

	inline std::vector<RigidBody*>::iterator bodies_begin() { return m_bodies.begin(); }
	inline std::vector<RigidBody*>::iterator bodies_end() { return m_bodies.end(); }
	inline std::vector<CollisionData>::iterator collisions_begin() { return m_collisions.begin(); }
	inline std::vector<CollisionData>::iterator collisions_end() { return m_collisions.end(); }
	
	void SolveCollision(CollisionData* collision);
	void PositionalCorrection(CollisionData* collision);

	void DebugDetectCollisions();

	inline CollisionDetector* GetCollisionDetector() { return &m_collisionDetector; }

	inline unsigned int GetNumIterations() const { return m_numIterations; }
	inline bool GetEnableFriction() const { return m_enableFriction; }
	inline bool GetEnableRestitution() const { return m_enableRestitution; }

	inline void SetNumIterations(unsigned int numIterations) { m_numIterations = numIterations; }
	inline void SetEnableFriction(bool enableFriction) { m_enableFriction = enableFriction; }
	inline void SetEnableRestitution(bool enableRestitution) { m_enableRestitution = enableRestitution; }


private:
	CollisionDetector m_collisionDetector;

	bool			m_enableFriction;
	bool			m_enableRestitution;
	unsigned int	m_numIterations;

	std::vector<RigidBody*> m_bodies;
	unsigned int m_idCounter;
	Vector3f m_gravity; // Global acceleration due to gravity.
	std::vector<CollisionData> m_collisions;
};


#endif // _CMG_PHYSICS_ENGINE_H_