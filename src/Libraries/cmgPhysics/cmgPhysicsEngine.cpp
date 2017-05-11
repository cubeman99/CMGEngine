#include "cmgPhysicsEngine.h"
#include <cmgMath/cmgMathLib.h>
#include <cmgMath/types/cmgPlane.h>
#include <cmgMath/types/cmgRay.h>


PhysicsEngine::PhysicsEngine() :
	m_gravity(Vector3f::ZERO),
	m_idCounter(0),
	m_enableFriction(true),
	m_enableRestitution(true),
	m_numIterations(3),
	m_profiler("Physics")
{
	m_gravity = Vector3f::DOWN * 9.81f;
}

PhysicsEngine::~PhysicsEngine()
{
	for (unsigned int i = 0; i < m_bodies.size(); ++i)
		delete m_bodies[i];
	m_bodies.clear();
}



void PhysicsEngine::Initialize()
{

}

void PhysicsEngine::AddBody(RigidBody* body)
{
	body->CalcInertia();
	m_bodies.push_back(body);
	body->m_id = m_idCounter;
	m_idCounter++;
}

void PhysicsEngine::ClearBodies()
{
	for (unsigned int i = 0; i < m_bodies.size(); ++i)
		delete m_bodies[i];
	m_bodies.clear();

	m_idCounter = 0;
}

void PhysicsEngine::RemoveBody(RigidBody* body)
{
	auto it = std::find(m_bodies.begin(), m_bodies.end(), body);
	if (it != m_bodies.end())
	{
		m_bodies.erase(it);
		delete body;
	}
}

void PhysicsEngine::Simulate(float timeDelta)
{
	ProfileSection* profileIntegration = m_profiler.GetSubSection("Integration");
	ProfileSection* profileDetection = m_profiler.GetSubSection("Collision Detection");
	ProfileSection* profilePositionalCorrection = m_profiler.GetSubSection("Positional Correction");
	ProfileSection* profileResponse = m_profiler.GetSubSection("Collision Response");

	m_profiler.Reset();
	m_profiler.StartInvocation();

	unsigned int i, j;
	RigidBody *body;
	CollisionData collisionData;

	timeDelta /= (float) m_numIterations;

	unsigned int iteration;
	for (iteration = 0; iteration < m_numIterations; ++iteration)
	{

	for (i = 0; i < m_bodies.size(); ++i)
	{
		body = m_bodies[i];
		body->CalculateDerivedData();
	}

	// Integrate.
	profileIntegration->StartInvocation();
	for (i = 0; i < m_bodies.size(); ++i)
	{
		body = m_bodies[i];
		
		if (body->m_inverseMass != 0.0f)
			body->m_velocity += m_gravity * timeDelta;

		body->IntegrateLinear(timeDelta);
		body->IntegrateAngular(timeDelta);
		body->CalculateDerivedData();
	}
	profileIntegration->StopInvocation();

	// Detect collisions.
	profileDetection->StartInvocation();
	m_collisions.clear();
	for (i = 0; i < m_bodies.size(); ++i)
	{
		for (j = i + 1; j < m_bodies.size(); ++j)
		{
			m_collisionDetector.DetectCollision(
				m_bodies[i], m_bodies[j], &collisionData);
			if (collisionData.numContacts > 0)
				m_collisions.push_back(collisionData);
		}
	}
	profileDetection->StopInvocation();

	// Resolve collisions.
	profileResponse->StartInvocation();
	for (i = 0; i < m_collisions.size(); ++i)
	{
		SolveCollision(&m_collisions[i]);
	}
	for (i = 0; i < m_bodies.size(); ++i)
	{
		body = m_bodies[i];
		body->m_velocity += body->m_velocityAccumulator;
		body->m_angularVelocity += body->m_angularVelocityAccumulator;
		body->ClearAccumulators();
	}
	profileResponse->StopInvocation();

	// Correct positions.
	profilePositionalCorrection->StartInvocation();
	for (i = 0; i < m_collisions.size(); ++i)
		PositionalCorrection(&m_collisions[i]);
	profilePositionalCorrection->StopInvocation();
	}

	m_profiler.StopInvocation();
}

void PhysicsEngine::SolveCollision(CollisionData* collision)
{
	for (unsigned int i = 0; i < collision->numContacts; ++i)
	{
		/*
		// NOTE: the contact normal will always face TOWARD body A.
		Contact& contact = collision->contacts[i];
		Vector3f contactNormal = contact.contactNormal;

		// TEMP: temp stuff.
		float restitution = 0.0f;
		
		// Calculate the contact point's position relative to each body's center of mass.
		Vector3f ra = contact.contactPoint - contact.bodyA->m_position;
		Vector3f rb = contact.contactPoint - contact.bodyB->m_position;

		// Calculate the velocity of the contact along its normal.
		// This is also called the closing speed.
		Vector3f relativeVelocity =
			(contact.bodyB->GetVelocity() + contact.bodyB->m_angularVelocity.Cross(rb)) - 
			(contact.bodyA->GetVelocity() + contact.bodyA->m_angularVelocity.Cross(ra));
		float contactVelocity = relativeVelocity.Dot(contactNormal);
		
		Vector3f contactPlanarVelocity = relativeVelocity - (contactNormal * contactVelocity);
		//contactPlanarVelocity.Normalize();

		// Do not resolve if objects are separating (closing speed < 0).
		if (contactVelocity < 0)
			continue;

		// Calculate the change in contact velocity per unit impulse.
		// This has linear and angular components.
		float velocityChangePerUnitImpulse = 0.0f;
		Vector3f deltaVelWorld = (contact.bodyA->m_inverseInertiaTensorWorld *
			ra.Cross(contactNormal)).Cross(ra);
		velocityChangePerUnitImpulse += deltaVelWorld.Dot(contactNormal);
		velocityChangePerUnitImpulse += contact.bodyA->m_inverseMass;
		deltaVelWorld = (contact.bodyB->m_inverseInertiaTensorWorld *
			rb.Cross(contactNormal)).Cross(rb);
		velocityChangePerUnitImpulse += deltaVelWorld.Dot(contactNormal);
		velocityChangePerUnitImpulse += contact.bodyB->m_inverseMass;

		// Calculate the desired contact velocity after applying the impulse.
		float desiredDeltaVelocity = -contactVelocity * (1.0f + restitution);
		
		// Now calculate the impulse.
		// This acts in the opposite direction of the contact normal.
		float impulseScalar = desiredDeltaVelocity / velocityChangePerUnitImpulse;
		//impulseScalar /= (float) collision->numContacts;
		Vector3f impulse = contactNormal * impulseScalar;

		//float dynamicDriction = 0.6f;
		//impulse += -contactPlanarVelocity * dynamicDriction * impulseScalar;
		//impulse += -contactPlanarVelocity * dynamicDriction * impulseScalar;

		//impulse += -contactPlanarVelocity / velocityChangePerUnitImpulse;
		*/

		Contact& contact = collision->contacts[i];
		contact.CalculateInternals(0.01667f);

		// Calculate the impulse in contact coordinates.
		Vector3f impulse;
		if (!m_enableFriction || (contact.staticFriction == 0.0f && contact.dynamicFriction == 0.0f))
			impulse = contact.CalculateFrictionlessImpulse();
		else
			impulse = contact.CalculateFrictionImpulse();

		// Scale the impulse by the number of contacts.
		impulse *= 1.0f / (float) collision->numContacts;
		
		// Transform the impulse into world coordinates.
		impulse = contact.contactToWorld * impulse;

		// Apply equal and opposite impulses to each body.
		contact.body[0]->ApplyImpulse(-impulse, contact.contactPoint);
		contact.body[1]->ApplyImpulse(impulse, contact.contactPoint);
	}
}

void PhysicsEngine::PositionalCorrection(CollisionData* collision)
{
	if (collision->numContacts > 0)
	{
		for (unsigned int i = 0; i < collision->numContacts; ++i)
		{
			Contact& contact = collision->contacts[i];
			Vector3f contactNormal = contact.contactNormal;
			float penetration = contact.penetration;
			float invMassSum = contact.body[0]->GetInverseMass() + contact.body[1]->GetInverseMass();

			//float k = 0.5f / (float) collision->numContacts;
			float k = 1.0f / (float) collision->numContacts;
			contact.body[0]->m_position += contactNormal * penetration * (contact.body[0]->m_inverseMass / invMassSum) * k;
			contact.body[1]->m_position -= contactNormal * penetration * (contact.body[1]->m_inverseMass / invMassSum) * k;
		}
	}
}

void PhysicsEngine::DebugDetectCollisions()
{
	unsigned int i, j;
	RigidBody* body;
	CollisionData collisionData;

	// Initialize frame.
	for (i = 0; i < m_bodies.size(); ++i)
	{
		body = m_bodies[i];
		body->CalculateDerivedData();
	}

	// Detect collisions.
	m_collisions.clear();
	for (i = 0; i < m_bodies.size(); ++i)
	{
		for (j = i + 1; j < m_bodies.size(); ++j)
		{
			m_collisionDetector.DetectCollision(m_bodies[i], m_bodies[j], &collisionData);
			if (collisionData.numContacts > 0)
				m_collisions.push_back(collisionData);
		}
	}
}
