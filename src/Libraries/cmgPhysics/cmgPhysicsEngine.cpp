#include "cmgPhysicsEngine.h"
#include <cmgMath/cmgMathLib.h>
#include <cmgMath/types/cmgPlane.h>
#include <cmgMath/types/cmgRay.h>


PhysicsEngine::PhysicsEngine() :
	m_gravity(Vector3f::ZERO),
	m_idCounter(0),
	m_enableFriction(true),
	m_enableRestitution(true),
	m_numIterations(1),
	m_profiler("Physics"),
	m_velocityIterations(6),
	m_positionIterations(3)

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


//-----------------------------------------------------------------------------
// Getters
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Setters
//-----------------------------------------------------------------------------

void PhysicsEngine::SetGravity(const Vector3f& gravity)
{
	m_gravity = gravity;
}


void PhysicsEngine::AddBody(RigidBody* body)
{
	body->CalcInertia();
	m_bodies.push_back(body);
	body->m_id = m_idCounter;
	m_idCounter++;
	body->m_physicsEngine = this;
}

void PhysicsEngine::ClearBodies()
{
	for (unsigned int i = 0; i < m_bodies.size(); ++i)
		delete m_bodies[i];
	m_bodies.clear();

	m_idCounter = 0;

	m_collisionCache.Clear();
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
	CollisionData cachedCollision;
	//CollisionData* collision;

	timeDelta /= (float) m_numIterations;

	float invDT = 1.0f / timeDelta;

	unsigned int iteration;
	for (iteration = 0; iteration < m_numIterations; ++iteration)
	{

	for (i = 0; i < m_bodies.size(); ++i)
	{
		body = m_bodies[i];
		body->CalculateDerivedData();
	}

	// Integrate velocities.
	profileIntegration->StartInvocation();
	for (i = 0; i < m_bodies.size(); ++i)
	{
		body = m_bodies[i];

		//body->IntegrateLinear(timeDelta);
		//body->IntegrateAngular(timeDelta);
		
		if (body->m_inverseMass != 0.0f)
			body->m_velocity += m_gravity * timeDelta;

		body->CalculateDerivedData();
	}
	profileIntegration->StopInvocation();

	// Detect collisions.
	profileDetection->StartInvocation();
	m_collisionCache.RefreshContacts();
	for (i = 0; i < m_bodies.size(); ++i)
	{
		for (j = i + 1; j < m_bodies.size(); ++j)
		{
			m_collisionDetector.DetectCollision(
				m_bodies[i], m_bodies[j], &collisionData);

			if (collisionData.numContacts > 0)
			{
				collisionData.CalcInternals();
				m_collisionCache.UpdateCollision(collisionData);
			}
		}
	}
	m_collisionCache.RemoveInactiveCollisions();
	profileDetection->StopInvocation();

	// Solve velocity constraints.
	profileResponse->StartInvocation();
	for (j = 0; j < m_velocityIterations; ++j)
	{
		for (auto it = m_collisionCache.collisions_begin();
			it != m_collisionCache.collisions_end(); ++it)
		{
			SolveCollision(&it->second);
			//collision->CalcInternals();
			//SolveCollision(collision);
		}
		//for (i = 0; i < m_bodies.size(); ++i)
		//{
		//	body = m_bodies[i];
		//	body->m_velocity += body->m_velocityAccumulator;
		//	body->m_angularVelocity += body->m_angularVelocityAccumulator;
		//	body->ClearAccumulators();
		//}
	}
	profileResponse->StopInvocation();

	// Integrate positions.
	profileIntegration->StartInvocation();
	for (i = 0; i < m_bodies.size(); ++i)
	{
		body = m_bodies[i];
		body->IntegrateLinear(timeDelta);
		body->IntegrateAngular(timeDelta);
		body->CalculateDerivedData();
		body->ClearAccumulators();
	}
	profileIntegration->StopInvocation();

	// Solve position constraints.
	profilePositionalCorrection->StartInvocation();
	for (j = 0; j < m_positionIterations; ++j)
	{
		for (auto it = m_collisionCache.collisions_begin();
			it != m_collisionCache.collisions_end(); ++it)
		{
			PositionalCorrection(&it->second, invDT);
		}
	}
	profilePositionalCorrection->StopInvocation();

	for (i = 0; i < m_bodies.size(); ++i)
	{
		body = m_bodies[i];
		body->CalculateDerivedData();
	}

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

		// Calculate the impulse in contact coordinates.
		Vector3f impulse;
		if (!m_enableFriction || (contact.staticFriction == 0.0f && contact.dynamicFriction == 0.0f))
			impulse = contact.CalculateFrictionlessImpulse();
		else
			impulse = contact.CalculateFrictionImpulse();

		// Scale the impulse by the number of contacts.
		//impulse *= 1.0f / (float) collision->numContacts;

		//if (contact.persistent)
			//impulse *= 0.5f;
		
		// Transform the impulse into world coordinates.
		impulse = contact.contactToWorld * impulse;
		
		Vector3f localToGlobalA = contact.bodyA->GetBodyToWorld().TransformAffine(contact.localPositionA);
		Vector3f localToGlobalB = contact.bodyB->GetBodyToWorld().TransformAffine(contact.localPositionB);
		//localToGlobalA = contact.worldPositionA;
		//localToGlobalB = contact.worldPositionB;

		// Apply equal and opposite impulses to each body.
		contact.bodyA->ApplyImpulse(-impulse, localToGlobalA);
		contact.bodyB->ApplyImpulse(impulse, localToGlobalB);

		contact.bodyA->CalculateDerivedData();
		contact.bodyB->CalculateDerivedData();
	}
}

void PhysicsEngine::PositionalCorrection(CollisionData* collision, float invDT)
{
	const float baumgarte = 0.2f;

	if (collision->numContacts > 0)
	{
		for (unsigned int i = 0; i < collision->numContacts; ++i)
		{
			Contact& contact = collision->contacts[i];

			// Normal points from B to A.

			/*Vector3f worldPosA = contact.bodyA->GetBodyToWorld()
				.TransformAffine(contact.localPositionA);
			Vector3f worldPosB = contact.bodyB->GetBodyToWorld()
				.TransformAffine(contact.localPositionB);
			Vector3f normal = contact.contactNormal;
			float depth = (worldPosB - worldPosA).Dot(normal);

			float b = depth * invDT * baumgarte;*/

			// Compute the effective mass
			//Vector3f rA = contact.contactPoint - contact.bodyA->m_centerOfMassWorld;
			//Vector3f rB = contact.contactPoint - contact.bodyA->m_centerOfMassWorld;
			//Vector3f rnA = rA.Cross(contactNormal);
			//Vector3f rnB = rB.Cross(contactNormal);
			//float effectiveMass =
			//	contact.bodyA->m_inverseMass + contact.bodyB->m_inverseMass +
			//	(contact.bodyA->m_inverseInertiaTensorWorld * (rnA * rnA)) +
			//	(contact.bodyB->m_inverseInertiaTensorWorld * (rnB * rnB));
			
			Vector3f localToGlobalA = contact.bodyA->GetBodyToWorld().TransformAffine(contact.localPositionA);
			Vector3f localToGlobalB = contact.bodyB->GetBodyToWorld().TransformAffine(contact.localPositionB);

			Vector3f impulse = contact.CalcBaumgarteImpulse() * contact.contactNormal;
			//contact.bodyA->ApplyImpulse(-impulse, contact.contactPoint);
			//contact.bodyB->ApplyImpulse(impulse, contact.contactPoint);

			float dt = 1.0f / invDT;

			contact.bodyA->m_position -= impulse * contact.bodyA->m_inverseMass * dt;
			contact.bodyB->m_position += impulse * contact.bodyB->m_inverseMass * dt;

			Vector3f angImpulseA = contact.bodyA->m_inverseInertiaTensorWorld * (localToGlobalA - contact.bodyA->m_centerOfMassWorld).Cross(-impulse);
			Vector3f angImpulseB = contact.bodyB->m_inverseInertiaTensorWorld * (localToGlobalB - contact.bodyB->m_centerOfMassWorld).Cross(impulse);
			contact.bodyA->IntegrateAngularVelocity(angImpulseA, dt);
			contact.bodyB->IntegrateAngularVelocity(angImpulseB, dt);
			
			contact.bodyA->CalculateDerivedData();
			contact.bodyB->CalculateDerivedData();

			/*
			Vector3f contactNormal = contact.contactNormal;
			float penetration = contact.penetration;
			float invMassSum = contact.body[0]->GetInverseMass() + contact.body[1]->GetInverseMass();

			//float k = 0.5f / (float) collision->numContacts;
			float k = 1.0f / (float) collision->numContacts;
			contact.body[0]->m_position += contactNormal * penetration * (contact.body[0]->m_inverseMass / invMassSum) * k;
			contact.body[1]->m_position -= contactNormal * penetration * (contact.body[1]->m_inverseMass / invMassSum) * k;
			*/
		}
	}
}

void PhysicsEngine::DebugDetectCollisions()
{
	unsigned int i;//, j;
	RigidBody* body;
	CollisionData collisionData;

	// Initialize frame.
	for (i = 0; i < m_bodies.size(); ++i)
	{
		body = m_bodies[i];
		body->CalculateDerivedData();
	}
}

bool PhysicsEngine::CastRay(const Ray& ray, float& outDistance, Vector3f& outNormal)
{
	outDistance = FLT_MAX;
	return CastBoundedRay(ray, outDistance, outNormal);
}

bool PhysicsEngine::CastBoundedRay(const Ray& ray, float& inOutDistance, Vector3f& outNormal)
{
	RigidBody* body;
	bool hit = false;

	for (unsigned int i = 0; i < m_bodies.size(); ++i)
	{
		body = m_bodies[i];
		Collider* collider = body->GetCollider();
		if (collider != nullptr &&
			collider->CastBoundedRay(ray, inOutDistance, outNormal))
		{
			hit = true;
		}
	}
	
	return hit;
}
