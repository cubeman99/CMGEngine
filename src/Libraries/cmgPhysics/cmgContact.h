#ifndef _CMG_PHYSICS_CONTACT_H_
#define _CMG_PHYSICS_CONTACT_H_

#include <cmgMath/types/cmgVector2f.h>
#include <cmgMath/types/cmgVector3f.h>
#include <cmgMath/types/cmgMatrix3f.h>

class RigidBody;


enum class ContactType
{
	k_vertex_face = 0,
	k_edge_edge,
	k_face_face, // at least one face must be curved.

	k_debug,

	k_count,
};


//-----------------------------------------------------------------------------
// Contact
//-----------------------------------------------------------------------------
struct Contact
{
public:
	
	Vector3f localPositionA;
	Vector3f worldPositionA;
	Vector3f localPositionB;
	Vector3f worldPositionB;
	bool persistent;

	// The two bodies in contact.
	// Index 0: The incident body (for contacts involving a face).
	// Index 1: The reference body (for contacts involving a face).
	union
	{
		struct { RigidBody* body[2]; };
		struct { RigidBody* bodyA; RigidBody* bodyB; };
	};
	
	// The position of the contact in world coordinates.
	Vector3f contactPoint;
	
	// The direction of the contact in world coordinates.
	// The contact normal should be facing toward the first body.
	Vector3f contactNormal;

	// How far are the two bodies inter-penetrating?
	// This value is negative for separated bodies.
	float penetration;

	// The coefficient of dynamic friction between the two objects.
	float dynamicFriction;

	// The coefficient of static friction between the two objects.
	float staticFriction;

	// The coefficient of restitution for the collision.
	float restitution;

	ContactType contactType;

	// The colliding features.
	//union
	struct
	{
		unsigned int faceIndex[2];

		struct
		{
			unsigned int vertexIndex[2];
		} edges[2];
	} features;

	
public:
	Contact();

	// Calculate and return the friction-less impulse in contact coordinates.
	Vector3f CalculateFrictionlessImpulse();

	// Calculate and return the friction impulse in contact coordinates.
	Vector3f CalculateFrictionImpulse();
	
	Vector3f TransformWorldToContact(const Vector3f& worldPoint) const;
	Vector3f TransformContactToWorld(const Vector3f& contactPoint) const;

protected:
public:

	// A transform matrix that converts coordinates in the contact's
	// frame of reference to world coordinates. The columns of this
	// matrix form an orthonormal set of vectors.
	Matrix3f contactToWorld;

	// Holds the closing velocity at the point of contact. This is set
	// when the CalculateInternals() function is run.
	Vector3f contactVelocity;

	// Holds the required change in velocity for this contact to be resolved.
	float desiredDeltaVelocity;

	// Holds the world space position of the contact point relative to the
	// center of each body. This is set when the CalculateInternals()
	// function is run.
	//Vector3f relativeContactPosition[2];
	
protected:
public:

	// Calculates internal data from state data. This is called before
	// the resolution algorithm tries to do any resolution. It should
	// never need to be called manually.
	void CalculateInternals(float timeDelta);

	void CreateContactBasis();
};


//-----------------------------------------------------------------------------
// CollisionData - a contact manifold
//-----------------------------------------------------------------------------
struct CollisionData
{
public:
	RigidBody*		firstBody;
	RigidBody*		secondBody;

	Contact			contacts[16];
	unsigned int	numContacts;
	float			friction;
	float			restitution;
	
	bool			active;

	CollisionData() :
		firstBody(nullptr),
		secondBody(nullptr)
	{}

	inline Contact* AddContact()
	{
		return &contacts[numContacts++];
	}
	
	inline Contact* AddContact(const Contact& contact)
	{
		Contact* addedContact = contacts + (numContacts++);
		*addedContact = contact;
		return addedContact;
	}

	inline void RemoveContact(unsigned int index)
	{
		--numContacts;

		// Shift other contacts down.
		for (unsigned int i = index; i < numContacts; ++i)
			contacts[i] = contacts[i + 1];
	}

	void CalcInternals();


protected:

};


#endif // _CMG_PHYSICS_CONTACT_H_