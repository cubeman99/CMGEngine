#include "cmgContact.h"
#include <cmgPhysics/cmgRigidBody.h>
#include <cmgMath/cmgMathLib.h>


Contact::Contact() :
	restitution(0.0f),
	staticFriction(0.3f),
	dynamicFriction(0.2f),
	persistent(false),
	age(0)
{
	bodyA = nullptr;
	bodyB = nullptr;
}

Vector3f Contact::CalculateFrictionlessImpulse()
{
	CalcDepth();

	if (penetration < 0.0f)
		return Vector3f::ZERO;
	
	Vector3f localToGlobalA = bodyA->GetBodyToWorld().TransformAffine(localPositionA);
	Vector3f localToGlobalB = bodyB->GetBodyToWorld().TransformAffine(localPositionB);

	// Calculate the contact point's position relative to each body's center of mass.
	Vector3f ra = localToGlobalA - bodyA->m_position;
	Vector3f rb = localToGlobalB - bodyB->m_position;
    // Find the relative velocity of the bodies at the contact point.
	contactVelocity = contactToWorld.GetTranspose() * 
		((bodyB->GetVelocity() + bodyB->m_angularVelocity.Cross(rb)) - 
		(bodyA->GetVelocity() + bodyA->m_angularVelocity.Cross(ra)));
    // Calculate the desired change in velocity for resolution
    //calculateDesiredDeltaVelocity(timeDelta);
	desiredDeltaVelocity = -contactVelocity.y;
	


	// NOTE: the contact normal will always face TOWARD body A.
		
	// Calculate the contact point's position relative to each body's center of mass.
	//Vector3f ra = contactPoint - bodyA->m_position;
	//Vector3f rb = contactPoint - bodyB->m_position;

	// Calculate the velocity of the contact along its normal.
	// This is also called the closing speed.
	Vector3f relativeVelocity =
		(bodyB->GetVelocity() + bodyB->m_angularVelocity.Cross(rb)) - 
		(bodyA->GetVelocity() + bodyA->m_angularVelocity.Cross(ra));
	float contactVelocity = relativeVelocity.Dot(contactNormal);

	// Do not resolve if objects are separating (closing speed < 0).
	if (contactVelocity < 0.0f)
		return Vector3f::ZERO;

	// Calculate the change in contact velocity per unit impulse.
	// This has linear and angular components.
	float velocityChangePerUnitImpulse = 0.0f;
	Vector3f deltaVelWorld = (bodyA->m_inverseInertiaTensorWorld *
		ra.Cross(contactNormal)).Cross(ra);
	velocityChangePerUnitImpulse += deltaVelWorld.Dot(contactNormal);
	velocityChangePerUnitImpulse += bodyA->m_inverseMass;
	deltaVelWorld = (bodyB->m_inverseInertiaTensorWorld *
		rb.Cross(contactNormal)).Cross(rb);
	velocityChangePerUnitImpulse += deltaVelWorld.Dot(contactNormal);
	velocityChangePerUnitImpulse += bodyB->m_inverseMass;

	// Calculate the desired contact velocity after applying the impulse.
	//float desiredDeltaVelocity = -contactVelocity * (1.0f + restitution);
		
	//float invDT = 1.0f / (1.0f / 60.0f);
	//float baumgarte = 0.1f;
	//desiredDeltaVelocity += -penetration * invDT * baumgarte;

	// Now calculate the impulse.
	// This acts in the opposite direction of the contact normal.
	float impulseScalar = desiredDeltaVelocity / velocityChangePerUnitImpulse;
	//Vector3f impulse = contactNormal * impulseScalar;
	Vector3f impulse(0.0f, impulseScalar, 0.0f);
	return impulse;
}
	
float Contact::CalcBaumgarteImpulse()
{
	CalcDepth();

	if (penetration < 0.0f)
		return 0.0f;
	
	Vector3f worldPosA = bodyA->GetBodyToWorld().TransformAffine(localPositionA);
	Vector3f worldPosB = bodyB->GetBodyToWorld().TransformAffine(localPositionB);
	Vector3f ra = worldPosA - bodyA->m_position;
	Vector3f rb = worldPosB - bodyB->m_position;
	
	// Calculate the change in contact velocity per unit impulse.
	// This has linear and angular components.
	float velocityChangePerUnitImpulse = 0.0f;
	Vector3f deltaVelWorld = (bodyA->m_inverseInertiaTensorWorld *
		ra.Cross(contactNormal)).Cross(ra);
	velocityChangePerUnitImpulse += deltaVelWorld.Dot(contactNormal);
	velocityChangePerUnitImpulse += bodyA->m_inverseMass;
	deltaVelWorld = (bodyB->m_inverseInertiaTensorWorld *
		rb.Cross(contactNormal)).Cross(rb);
	velocityChangePerUnitImpulse += deltaVelWorld.Dot(contactNormal);
	velocityChangePerUnitImpulse += bodyB->m_inverseMass;

	float invDT = 1.0f / (1.0f / 60.0f);
	float baumgarte = 0.9f;
	float slop = 0.01f;
	float b = -baumgarte * invDT * Math::Max(penetration - slop, 0.0f);
	return (b / velocityChangePerUnitImpulse);
}

Vector3f Contact::CalculateFrictionImpulse()
{
	CalcDepth();

	if (penetration < 0.0f)
		return Vector3f::ZERO;

	// Calculate the contact point's position relative to each body's center of mass.
	Vector3f ra = worldPositionA - bodyA->m_position;
	Vector3f rb = worldPositionB - bodyB->m_position;
    // Find the relative velocity of the bodies at the contact point.
	contactVelocity = contactToWorld.GetTranspose() * 
		((bodyB->GetVelocity() + bodyB->m_angularVelocity.Cross(rb)) - 
		(bodyA->GetVelocity() + bodyA->m_angularVelocity.Cross(ra)));
    // Calculate the desired change in velocity for resolution
    //calculateDesiredDeltaVelocity(timeDelta);
	desiredDeltaVelocity = -contactVelocity.y;
	


	// Don't apply impulses to seperating contacts.
	if (contactVelocity.y < 0.0f)
		return Vector3f::ZERO;

	// NOTE: the contact normal will always face TOWARD body A.
		
	float inverseMassSum = bodyA->GetInverseMass() + bodyB->GetInverseMass();

	//-------------------------------------------------------------------------
	// First body

	// The equivalent of a cross-product in matrices is multiplication
	// by a skew-symmetric matrix. We build the matrix for converting
	// between linear and angular velocity.
	Matrix3f impulseToTorque;
	//Vector3f ra = contactPoint - bodyA->m_position;
	impulseToTorque.InitSkewSymmetric(ra);

	// Build the matrix to convert contact impulse to change in velocity
	// in world coordinates.
	Matrix3f deltaVelWorld = impulseToTorque;
	deltaVelWorld = deltaVelWorld * bodyA->GetInverseInertiaTensor();
	deltaVelWorld = deltaVelWorld * impulseToTorque;
	deltaVelWorld.Negate();

	//-------------------------------------------------------------------------
	// Second body

	// The equivalent of a cross-product in matrices is multiplication
	// by a skew-symmetric matrix. We build the matrix for converting
	// between linear and angular velocity.
	//Vector3f rb = contactPoint - bodyB->m_position;
	impulseToTorque.InitSkewSymmetric(rb);

	// Build the matrix to convert contact impulse to change in velocity
	// in world coordinates.
	Matrix3f deltaVelWorld2 = impulseToTorque;
	deltaVelWorld2 = deltaVelWorld2 * bodyB->GetInverseInertiaTensor();
	deltaVelWorld2 = deltaVelWorld2 * impulseToTorque;
	deltaVelWorld2.Negate();

	deltaVelWorld += deltaVelWorld2;

	//-------------------------------------------------------------------------
	// Impulse

	// Perform a change of basis to convert into contact coordinates.
	Matrix3f deltaVelocity = contactToWorld.GetTranspose();
	deltaVelocity = deltaVelocity * deltaVelWorld;
	deltaVelocity = deltaVelocity * contactToWorld;

	// Add in the linear velocity change.
	deltaVelocity += Matrix3f::CreateScale3(inverseMassSum);

	// Invert to get the impulse needed per unit velocity.
	Matrix3f impulseMatrix = deltaVelocity.GetInverse();
	
	// Find the target velocities to kill.
	Vector3f velKill(-contactVelocity.x,
					desiredDeltaVelocity,
					-contactVelocity.z);

	// Find the impulse to kill target velocities.
	Vector3f impulseContact = impulseMatrix.Transform(velKill);

	//-------------------------------------------------------------------------
	// Dynamic friction

	// Check for exceeding static friction.
	float planarImpulse = impulseContact.GetXZ().Length();
	if (planarImpulse > Math::Abs(impulseContact.y) * staticFriction)
	{
		impulseContact.x /= planarImpulse;
		impulseContact.z /= planarImpulse;
		impulseContact.y = deltaVelocity.m[3] * dynamicFriction * impulseContact.x +
							deltaVelocity.m[4] +
							deltaVelocity.m[5] * dynamicFriction * impulseContact.z;
		impulseContact.y = desiredDeltaVelocity / impulseContact.y;
		impulseContact.x *= dynamicFriction * Math::Abs(impulseContact.y);
		impulseContact.z *= dynamicFriction * Math::Abs(impulseContact.y);
	}

	return impulseContact;
}

float Contact::CalcDepth()
{
	contactNormal = bodyB->GetBodyToWorld().Rotate(localNormal);
	//Vector3f worldPosA = bodyA->GetBodyToWorld().TransformAffine(localPositionA);
	//Vector3f worldPosB = bodyB->GetBodyToWorld().TransformAffine(localPositionB);
	//penetration = (worldPosB - worldPosA).Dot(contactNormal);

	
	worldPositionA = bodyA->GetBodyToWorld()
		.TransformAffine(localPositionA);
	worldPositionB = bodyB->GetBodyToWorld()
		.TransformAffine(localPositionB);
	penetration = (worldPositionB -
		worldPositionA).Dot(contactNormal);

	return penetration;
}

void Contact::CalculateInternals(float timeDelta)
{
	contactNormal = bodyB->GetBodyToWorld().Rotate(localNormal);
	//worldPositionA = contactPoint;
	//localPositionA = bodyA->GetWorldToBody().TransformAffine(contactPoint);
	//worldPositionB = contactPoint;
	//localPositionB = bodyB->GetWorldToBody().TransformAffine(contactPoint);
	persistent = false;
	
	restitution = 0.0f;
	staticFriction = 0.4f;
	dynamicFriction = 0.3f;
	//staticFriction = 0.0f;
	//dynamicFriction = 0.0f;

    // Calculate an set of axis at the contact point.
	CreateContactBasis();
	
	// Calculate the contact point's position relative to each body's center of mass.
	Vector3f ra = contactPoint - bodyA->m_position;
	Vector3f rb = contactPoint - bodyB->m_position;

    // Find the relative velocity of the bodies at the contact point.
	contactVelocity = contactToWorld.GetTranspose() * 
		((bodyB->GetVelocity() + bodyB->m_angularVelocity.Cross(rb)) - 
		(bodyA->GetVelocity() + bodyA->m_angularVelocity.Cross(ra)));

    // Calculate the desired change in velocity for resolution
    //calculateDesiredDeltaVelocity(timeDelta);
	desiredDeltaVelocity = -contactVelocity.y;
}

void Contact::CreateContactBasis()
{
	// Use either the Y-axis or Z-axis to create the orthonormal basis
	// with, choosing whichever is less parallel to the contact normal.
	Vector3f contactTangent;
	if (Math::Abs(contactNormal.x) > Math::Abs(contactNormal.z))
		contactTangent = Vector3f::UNITZ;
	else
		contactTangent = Vector3f::UNITX;
	
	// Create an orthonormal basis for the contact.
	Vector3f contactBitangent = contactNormal.Cross(contactTangent);
	contactBitangent.Normalize();
	contactTangent = contactBitangent.Cross(contactNormal);
	contactTangent.Normalize();

	// Create the basis matrix.
	// The contact normal is along the Y-axis.
	contactToWorld.Set(
		contactTangent,
		contactNormal,
		contactBitangent);
}





void CollisionData::CalcInternals()
{
	for (unsigned int i = 0; i < numContacts; ++i)
	{
		contacts[i].CalculateInternals(0.0f);
	}
}


// modified from bullet3
void CollisionData::RefreshContacts()
{
	// Refresh positions and distance.
	for (unsigned int i = 0; i < numContacts; ++i)
	{
		Contact& contact = contacts[i];
		contact.worldPositionA = contact.bodyA->GetBodyToWorld()
			.TransformAffine(contact.localPositionA);
		contact.worldPositionB = contact.bodyB->GetBodyToWorld()
			.TransformAffine(contact.localPositionB);
		contact.penetration = (contact.worldPositionB -
			contact.worldPositionA).Dot(contact.contactNormal);
		contact.age++;
		contact.persistent = true;
	}

	float maxDist = 0.2f;

	Vector3f projectedDifference;
	Vector3f projectedPoint;
	float planarDistance;

	// Remove contacts that have moved too far.
	for (unsigned int i = 0; i < numContacts; ++i)
	{
		Contact& contact = contacts[i];

		// Contact becomes invalid when signed distance
		// exceeds margin (projected on contactnormal direction)
		if (contact.penetration < -maxDist)
		{
			RemoveContact(i--);
		}
		else
		{
			// Contact also becomes invalid when relative
			// movement orthogonal to normal exceeds margin.
			projectedPoint = contact.worldPositionA - (contact.contactNormal * -contact.penetration);
			projectedDifference = contact.worldPositionB - projectedPoint;
			planarDistance = projectedDifference.LengthSquared();

			if (planarDistance > maxDist * maxDist)
			{
				RemoveContact(i--);
			}
		}
	}
}

