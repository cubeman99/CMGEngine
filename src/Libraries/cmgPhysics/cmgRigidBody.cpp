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
	m_force(Vector3f::ZERO),
	m_torque(Vector3f::ZERO),
	m_acceleration(Vector3f::ZERO),
	m_angularAcceleration(Vector3f::ZERO),
	m_centerOfMass(Vector3f::ZERO),
	m_physicsEngine(nullptr)
{
}

RigidBody::~RigidBody()
{
	// Delete colliders.
	for (unsigned int i = 0; i < m_colliders.size(); ++i)
		delete m_colliders[i];
	m_colliders.clear();
}

Vector3f RigidBody::GetVelocityAtPoint(const Vector3f& pointWorld) const
{
	return m_velocity + m_angularVelocity.Cross(pointWorld - m_centerOfMassWorld);
}

void RigidBody::SetMass(float mass)
{
	m_mass = mass;

	if (mass > 0.0f)
		m_inverseMass = 1.0f / mass;
	else
		m_inverseMass = 0.0f;
}

void RigidBody::SetCollider(Collider* collider)
{
	AddCollider(collider);
}

void RigidBody::AddCollider(Collider* collider)
{
	collider->m_body = this;
	m_colliders.push_back(collider);
}

void RigidBody::ClearColliders()
{
	// Delete colliders.
	for (unsigned int i = 0; i < m_colliders.size(); ++i)
		delete m_colliders[i];
	m_colliders.clear();
}


void RigidBody::ApplyForce(const Vector3f& force, const Vector3f& contactPoint)
{
	m_force += force;
	m_torque += Vector3f::Cross(contactPoint - m_centerOfMassWorld, force);
}

void RigidBody::ApplyForceLinear(const Vector3f& force)
{
	m_force += force;
}

void RigidBody::ApplyTorque(const Vector3f& torque)
{
	m_torque += torque;
}

void RigidBody::ApplyImpulse(const Vector3f& impulse, const Vector3f& contactPoint)
{
	//m_velocityAccumulator += impulse * m_inverseMass;
	//m_angularVelocityAccumulator += m_inverseInertiaTensorWorld * (contactPoint - m_centerOfMassWorld).Cross(impulse);
	m_velocity += impulse * m_inverseMass;
	m_angularVelocity += m_inverseInertiaTensorWorld * (contactPoint - m_centerOfMassWorld).Cross(impulse);
}

void RigidBody::CalculateDerivedData()
{
	m_bodyToWorld.InitRotation(m_orientation);
	m_bodyToWorld.SetTranslation(m_position);
	m_inverseInertiaTensorWorld = m_bodyToWorld.Get3x3() *
		m_inverseInertiaTensor * m_bodyToWorld.Get3x3().GetTranspose();
	m_worldToBody = m_bodyToWorld.GetAffineInverse();

	m_centerOfMassWorld = m_bodyToWorld.TransformAffine(m_centerOfMass);
	
	for (unsigned int i = 0; i < m_colliders.size(); ++i)
		m_colliders[i]->CalcDerivedData();

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
	m_force.SetZero();
	m_torque.SetZero();
}




void RigidBody::IntegrateLinear(float dt)
{
	m_acceleration = m_inverseMass * m_force;
	//body->force.set(); // clear force accumulator.

	// First-order integration
	m_position += m_velocity * dt;
	//m_velocity += m_acceleration * dt;
	m_velocity += m_velocityAccumulator + (m_acceleration * dt);
}

void RigidBody::IntegrateAngular(float dt)
{
	// We need to update these in order to rotate the body around it's center of mass.
	m_bodyToWorld.InitRotation(m_orientation);
	m_bodyToWorld.c3.xyz = m_position;
	m_centerOfMassWorld = m_bodyToWorld.TransformAffine(m_centerOfMass);
	
	m_angularAcceleration = m_inverseInertiaTensorWorld * m_torque;
	//m_torque.SetZero(); // clear torque accumulator
	m_angularVelocity += m_angularVelocityAccumulator + (m_angularAcceleration * dt);
	
	// First-order angular velocity integration
	Quaternion angularVelocityQuat;
	angularVelocityQuat.xyz = m_angularVelocity;
	angularVelocityQuat.w = 0.0f;
	m_orientation += (angularVelocityQuat * m_orientation) * (dt * 0.5f);
	//m_angularVelocity += m_angularAcceleration * dt;

	// Rotate the body's origin around its center of mass.
	Vector3f momentArm = m_position - m_centerOfMassWorld;
	float wLength = m_angularVelocity.Length();
	if (wLength > 0.0f && momentArm.LengthSquared() > 0.0f)
	{
		Quaternion originRotation = Quaternion(
			m_angularVelocity / wLength, wLength * dt);
		m_position -= m_centerOfMassWorld;
		//m_position = originRotation.RotateVector(m_position);
		originRotation.RotateVector(m_position);
		m_position += m_centerOfMassWorld;
	}

	// corrections
	m_orientation.Normalize();
	//m_angularVelocityQuat.set(body->angularVelocity, 0.0f);
}


void RigidBody::IntegrateAngularVelocity(const Vector3f& angularVelocity, float amount)
{
	// We need to update these in order to rotate the body around it's center of mass.
	m_bodyToWorld.InitRotation(m_orientation);
	m_bodyToWorld.c3.xyz = m_position;
	m_centerOfMassWorld = m_bodyToWorld.TransformAffine(m_centerOfMass);

	// First-order angular velocity integration
	Quaternion angularVelocityQuat;
	angularVelocityQuat.xyz = angularVelocity;
	angularVelocityQuat.w = 0.0f;
	m_orientation += (angularVelocityQuat * m_orientation) * (amount * 0.5f);

	// Rotate the body's origin around its center of mass.
	Vector3f momentArm = m_position - m_centerOfMassWorld;
	float wLength = angularVelocity.Length();
	if (wLength > 0.0f && momentArm.LengthSquared() > 0.0f)
	{
		Quaternion originRotation = Quaternion(
			angularVelocity / wLength, wLength * amount);
		m_position -= m_centerOfMassWorld;
		originRotation.RotateVector(m_position);
		m_position += m_centerOfMassWorld;
	}

	m_orientation.Normalize();
}

