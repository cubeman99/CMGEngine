#include "cmgRigidBody.h"


RigidBody::RigidBody() :
	m_physicsMesh(this),
	m_position(Vector3f::ZERO),
	m_orientation(Quaternion::IDENTITY),
	m_velocity(Vector3f::ZERO),
	m_angularVelocity(Vector3f::ZERO),
	m_inverseMass(1.0f),
	m_inverseInertiaTensor(Matrix3f::IDENTITY),
	m_restitution(0.0f),
	m_staticFriction(0.0f),
	m_dynamicFriction(0.0f),
	m_velocityAccumulator(0.0f),
	m_angularVelocityAccumulator(0.0f),
	m_collider(nullptr)
{
}

void RigidBody::SetMass(float mass)
{
	if (mass > 0.0f)
		m_inverseMass = 1.0f / mass;
	else
		m_inverseMass = 0.0f;
}

void RigidBody::SetCollider(Collider* collider)
{
	// Remove existing collider.
	if (m_collider != nullptr)
	{
		m_collider->m_body = nullptr;
		delete m_collider;
	}

	collider->m_body = this;
	m_collider = collider;
}


void RigidBody::ApplyImpulse(const Vector3f& impulse, const Vector3f& contactPoint)
{
	m_velocityAccumulator += impulse * m_inverseMass;
	m_angularVelocityAccumulator += m_inverseInertiaTensorWorld * contactPoint.Cross(impulse);
	//m_velocity += impulse * m_inverseMass;
	//m_angularVelocity += m_inverseInertiaTensorWorld * contactPoint.Cross(impulse);
}

void RigidBody::CalculateDerivedData()
{
	m_bodyToWorld.InitRotation(m_orientation);
	m_bodyToWorld.SetTranslation(m_position);
	m_inverseInertiaTensorWorld = m_bodyToWorld.Get3x3() *
		m_inverseInertiaTensor * m_bodyToWorld.Get3x3().GetTranspose();

	m_worldToBody = m_bodyToWorld.GetAffineInverse();

	if (m_collider != nullptr)
		m_collider->CalcDerivedData();

	//m_inverseInertiaTensor.InitScale(1.0f);
	
	if (m_inverseMass == 0.0f)
	{
		//m_inverseInertiaTensor.SetZero();
		//m_inverseInertiaTensorWorld.SetZero();
		//if (m_id == 1)
		//{
		//	m_primitive->CalculateInertiaTensor(
		//		20.0f / 1.0f,
		//		m_inverseInertiaTensor);
		//	m_inverseInertiaTensor.Invert();
		//	m_inverseInertiaTensor[4] = 0.0f;
		//	m_inverseInertiaTensor[8] = 0.0f;
		//	m_inverseInertiaTensorWorld = m_bodyToWorld.Get3x3() * m_inverseInertiaTensor * m_bodyToWorld.Get3x3().GetTranspose();
		//}
	}
	else
	{
		/*
		if (m_primitives.size() > 0)
		{
			float volume = 0.0f;
			for (unsigned int i = 0; i < m_primitives.size(); i++)
				volume += m_primitives[i]->GetVolume();
			float density = (1.0f / m_inverseMass) / volume;

			float inertiaScale = 10.0f; // TEMP: scale inertia

			Matrix3f totalInertia = Matrix3f::ZERO;
			for (unsigned int i = 0; i < m_primitives.size(); i++)
			{
				CollisionPrimitive* primitive = m_primitives[i];
				float primitiveMass = density * primitive->GetVolume();
				Matrix3f localInertiaTensor;
				primitive->CalculateInertiaTensor(inertiaScale * primitiveMass, localInertiaTensor);
				localInertiaTensor = primitive->offset.Get3x3().GetTranspose() * localInertiaTensor;
				Vector3f offsetFromCenterOfMass = primitive->offset.GetTranslation();
				totalInertia += localInertiaTensor + Matrix3f::CreateScale(offsetFromCenterOfMass * offsetFromCenterOfMass * primitiveMass);
			}
			m_inverseInertiaTensor = totalInertia;
			m_inverseInertiaTensor.Invert();
		}*/

		//if (m_primitives.size() > 0)
		//{
		//	m_primitives[0]->CalculateInertiaTensor(
		//		20.0f / m_inverseMass,
		//		m_inverseInertiaTensor);
		//	m_inverseInertiaTensor.Invert();
		//}
		//m_inverseInertiaTensorWorld = m_bodyToWorld.Get3x3() * m_inverseInertiaTensor * m_bodyToWorld.Get3x3().GetTranspose();
	}
}

void RigidBody::ClearAccumulators()
{
	m_velocityAccumulator.SetZero();
	m_angularVelocityAccumulator.SetZero();
}