#include "cmgContact.h"
#include <cmgPhysics/cmgRigidBody.h>
#include <cmgMath/cmgMathLib.h>


Contact::Contact()
{
	body[0] = nullptr;
	body[1] = nullptr;
	restitution = 0.0f;
	staticFriction = 0.3f;
	dynamicFriction = 0.2f;
}

Vector3f Contact::CalculateFrictionlessImpulse()
{
	// NOTE: the contact normal will always face TOWARD body A.
		
	// Calculate the contact point's position relative to each body's center of mass.
	Vector3f ra = contactPoint - body[0]->m_position;
	Vector3f rb = contactPoint - body[1]->m_position;

	// Calculate the velocity of the contact along its normal.
	// This is also called the closing speed.
	Vector3f relativeVelocity =
		(body[1]->GetVelocity() + body[1]->m_angularVelocity.Cross(rb)) - 
		(body[0]->GetVelocity() + body[0]->m_angularVelocity.Cross(ra));
	float contactVelocity = relativeVelocity.Dot(contactNormal);

	// Do not resolve if objects are separating (closing speed < 0).
	if (contactVelocity < 0.0f)
		return Vector3f::ZERO;

	// Calculate the change in contact velocity per unit impulse.
	// This has linear and angular components.
	float velocityChangePerUnitImpulse = 0.0f;
	Vector3f deltaVelWorld = (body[0]->m_inverseInertiaTensorWorld *
		ra.Cross(contactNormal)).Cross(ra);
	velocityChangePerUnitImpulse += deltaVelWorld.Dot(contactNormal);
	velocityChangePerUnitImpulse += body[0]->m_inverseMass;
	deltaVelWorld = (body[1]->m_inverseInertiaTensorWorld *
		rb.Cross(contactNormal)).Cross(rb);
	velocityChangePerUnitImpulse += deltaVelWorld.Dot(contactNormal);
	velocityChangePerUnitImpulse += body[1]->m_inverseMass;

	// Calculate the desired contact velocity after applying the impulse.
	//float desiredDeltaVelocity = -contactVelocity * (1.0f + restitution);
		
	// Now calculate the impulse.
	// This acts in the opposite direction of the contact normal.
	float impulseScalar = desiredDeltaVelocity / velocityChangePerUnitImpulse;
	//Vector3f impulse = contactNormal * impulseScalar;
	Vector3f impulse(0.0f, impulseScalar, 0.0f);
	return impulse;
}

Vector3f Contact::CalculateFrictionImpulse()
{
	// Don't apply impulses to seperating contacts.
	if (contactVelocity.y < 0.0f)
		return Vector3f::ZERO;

	// NOTE: the contact normal will always face TOWARD body A.
		
	float inverseMassSum = body[0]->GetInverseMass() + body[1]->GetInverseMass();

	//-------------------------------------------------------------------------
	// First body

	// The equivalent of a cross-product in matrices is multiplication
	// by a skew-symmetric matrix. We build the matrix for converting
	// between linear and angular velocity.
	Matrix3f impulseToTorque;
	Vector3f ra = contactPoint - body[0]->m_position;
	impulseToTorque.InitSkewSymmetric(ra);

	// Build the matrix to convert contact impulse to change in velocity
	// in world coordinates.
	Matrix3f deltaVelWorld = impulseToTorque;
	deltaVelWorld = deltaVelWorld * body[0]->GetInverseInertiaTensor();
	deltaVelWorld = deltaVelWorld * impulseToTorque;
	deltaVelWorld.Negate();

	//-------------------------------------------------------------------------
	// Second body

	// The equivalent of a cross-product in matrices is multiplication
	// by a skew-symmetric matrix. We build the matrix for converting
	// between linear and angular velocity.
	Vector3f rb = contactPoint - body[1]->m_position;
	impulseToTorque.InitSkewSymmetric(rb);

	// Build the matrix to convert contact impulse to change in velocity
	// in world coordinates.
	Matrix3f deltaVelWorld2 = impulseToTorque;
	deltaVelWorld2 = deltaVelWorld2 * body[1]->GetInverseInertiaTensor();
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
	deltaVelocity += Matrix3f::CreateScale(inverseMassSum);

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


void Contact::CalculateInternals(float timeDelta)
{
    // Calculate an set of axis at the contact point.
	CreateContactBasis();
	
	// Calculate the contact point's position relative to each body's center of mass.
	Vector3f ra = contactPoint - body[0]->m_position;
	Vector3f rb = contactPoint - body[1]->m_position;

    // Find the relative velocity of the bodies at the contact point.
	contactVelocity = contactToWorld.GetTranspose() * 
		((body[1]->GetVelocity() + body[1]->m_angularVelocity.Cross(rb)) - 
		(body[0]->GetVelocity() + body[0]->m_angularVelocity.Cross(ra)));

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


