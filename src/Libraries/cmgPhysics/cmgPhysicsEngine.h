#ifndef _CMG_PHYSICS_ENGINE_H_
#define _CMG_PHYSICS_ENGINE_H_

#include <cmgMath/types/cmgVector4f.h>
#include <cmgPhysics/cmgContact.h>
#include <cmgPhysics/cmgRigidBody.h>
#include <cmgPhysics/cmgCollisionDetector.h>
#include <cmgPhysics/cmgCollisionCache.h>
#include <cmgCore/time/cmgTimer.h>


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
	void RemoveBody(RigidBody* body);
	void ClearBodies();

	// Getters
	inline unsigned int GetNumBodies() const { return m_bodies.size(); }
	inline RigidBody* GetBody(unsigned int index) { return m_bodies[index]; }
	inline std::vector<RigidBody*>::iterator bodies_begin() { return m_bodies.begin(); }
	inline std::vector<RigidBody*>::iterator bodies_end() { return m_bodies.end(); }
	inline CollisionCache* GetCollisionCache() { return &m_collisionCache; }
	inline CollisionDetector* GetCollisionDetector() { return &m_collisionDetector; }
	inline ProfileSection* GetProfiler() { return &m_profiler; }
	
	inline unsigned int GetNumIterations() const { return m_numIterations; }
	inline bool GetEnableFriction() const { return m_enableFriction; }
	inline bool GetEnableRestitution() const { return m_enableRestitution; }

	// Setters
	inline void SetNumIterations(unsigned int numIterations) { m_numIterations = numIterations; }
	inline void SetEnableFriction(bool enableFriction) { m_enableFriction = enableFriction; }
	inline void SetEnableRestitution(bool enableRestitution) { m_enableRestitution = enableRestitution; }
	void SetGravity(const Vector3f& gravity);
	
	void SolveCollision(CollisionData* collision);
	void PositionalCorrection(CollisionData* collision, float invDT);
	void DebugDetectCollisions();

	// Operations
	bool CastRay(const Ray& ray, float& outDistance, Vector3f& outNormal);
	bool CastBoundedRay(const Ray& ray, float& inOutDistance, Vector3f& outNormal);

private:
	CollisionDetector m_collisionDetector;

	bool			m_enableFriction;
	bool			m_enableRestitution;
	unsigned int	m_numIterations;

	unsigned int	m_velocityIterations;
	unsigned int	m_positionIterations;

	CollisionCache m_collisionCache;

	std::vector<RigidBody*> m_bodies;
	unsigned int m_idCounter;
	Vector3f m_gravity; // Global acceleration due to gravity.

	ProfileSection m_profiler;
};


#endif // _CMG_PHYSICS_ENGINE_H_