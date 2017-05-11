#ifndef _CMG_PHYSICS_RIGID_BODY_H_
#define _CMG_PHYSICS_RIGID_BODY_H_

#include <cmgMath/types/cmgVector3f.h>
#include <cmgMath/types/cmgQuaternion.h>
#include <cmgMath/types/cmgMatrix3f.h>
#include <cmgMath/types/cmgMatrix4f.h>
#include <cmgCore/containers/cmgArray.h>
#include "cmgPhysicsMesh.h"
#include <cmgPhysics/colliders/cmgCollider.h>


//-----------------------------------------------------------------------------
// RigidBody
//-----------------------------------------------------------------------------
class RigidBody
{
	friend class PhysicsEngine;

public:
	// Constructors
	RigidBody();

	// Getters
	inline const Vector3f& GetPosition() const { return m_position; }
	inline const Quaternion& GetOrientation() const { return m_orientation; }
	inline const Vector3f& GetVelocity() const { return m_velocity; }
	inline const Vector3f& GetAngularVelocity() const { return m_angularVelocity; }
	inline float GetInverseMass() const { return m_inverseMass; }
	inline const Matrix3f& GetInverseInertiaTensor() const { return m_inverseInertiaTensor; }
	inline PhysicsMesh* GetPhysicsMesh() { return &m_physicsMesh; }
	inline unsigned int GetId() const { return m_id; }
	inline Collider* GetCollider() { return m_collider; }

	inline const Matrix4f& GetBodyToWorld() const { return m_bodyToWorld; }
	inline const Matrix4f& GetWorldToBody() const { return m_worldToBody; }


	// Setters
	inline void SetPosition(const Vector3f& position) { m_position = position; }
	inline void SetOrientation(const Quaternion& orientation) { m_orientation = orientation; }
	inline void SetVelocity(const Vector3f& velocity) { m_velocity = velocity; }
	inline void SetAngularVelocity(const Vector3f& angularVelocity) { m_angularVelocity = angularVelocity; }
	inline void SetInverseMass(float inverseMass) { m_inverseMass = inverseMass; }
	inline void SetInverseInertiaTensor(const Matrix3f& inverseInertiaTensor) { m_inverseInertiaTensor = inverseInertiaTensor; }
	inline void SetRestitution(float restitution) { m_restitution = restitution; }
	void SetMass(float mass);
	void SetCollider(Collider* collider);

	/*inline void SetPrimitive(CollisionPrimitive* primitive)
	{
		AddPrimitive(primitive, Matrix4f::IDENTITY);
	}
	inline void AddPrimitive(CollisionPrimitive* primitive, const Matrix4f& offset)
	{
		primitive->body = this;
		primitive->bodyToShape = offset;
		m_physicsMesh.AddShape(primitive);
	}
	inline void AddPrimitive(CollisionPrimitive* primitive, const Vector3f& offset)
	{
		primitive->body = this;
		primitive->bodyToShape = Matrix4f::CreateTranslation(offset);
		m_physicsMesh.AddShape(primitive);
	}
	*/
	inline std::vector<CollisionPrimitive*>::iterator primitives_begin() { return m_physicsMesh.shapes_begin(); }
	inline std::vector<CollisionPrimitive*>::iterator primitives_end() { return m_physicsMesh.shapes_end(); }


	void ApplyImpulse(const Vector3f& impulse, const Vector3f& contactPoint);
	void CalculateDerivedData();
	void ClearAccumulators();

	void CalcInertia()
	{
		// Rigid body origin must be aligned with the center of mass.
		//m_physicsMesh.ShiftOriginToCenterOfMass();

		// Calculate the inverse inertia tensor from the geometry.
		if (m_inverseMass == 0.0f)
		{
			m_inverseInertiaTensor.SetZero();
		}
		else
		{
			m_inverseInertiaTensor = m_collider->CalcInertiaTensor(1.0f / m_inverseMass);
			m_inverseInertiaTensor.Invert();
			//m_physicsMesh.CalcInverseInertiaTensor(
				//1.0f / m_inverseMass, m_inverseInertiaTensor);
		}
	}

private:
public:

	unsigned int	m_id;

	// Dynamics
	Vector3f		m_position;
	Quaternion		m_orientation;
	Vector3f		m_velocity;
	Vector3f		m_angularVelocity;

	Vector3f		m_velocityAccumulator;
	Vector3f		m_angularVelocityAccumulator;

	// Geometry
	PhysicsMesh		m_physicsMesh;
	Collider*		m_collider;

	// Material properties
	float			m_inverseMass;
	Matrix3f		m_inverseInertiaTensor;
	float			m_restitution;
	float			m_staticFriction;
	float			m_dynamicFriction;

public:
	// Derived data
	Matrix4f		m_bodyToWorld;
	Matrix4f		m_worldToBody;
	Matrix3f		m_inverseInertiaTensorWorld; // inverse inertia tensor in world space.
};


#endif // _CMG_PHYSICS_RIGID_BODY_H_