#include "cmgCollisionCache.h"
#include <cmgPhysics/cmgRigidBody.h>


CollisionCache::CollisionCache()
{
	m_persistentThresholdSqr = 0.3f;
	m_persistentThresholdSqr *= m_persistentThresholdSqr;
	m_maxContacts = 4;
	m_enableCaching = false;
}

CollisionCache::~CollisionCache()
{
}

void CollisionCache::Clear()
{
	m_collisions.clear();
}

void CollisionCache::RefreshContacts()
{
	if (m_enableCaching)
	{
		for (auto it = m_collisions.begin(); it != m_collisions.end(); )
		{
			it->second.RefreshContacts();
			
			if (it->second.numContacts == 0)
				m_collisions.erase(it++);
			else
				++it;
		}
	}
	else
	{
		m_collisions.clear();
	}
}

void CollisionCache::UpdateCollision(const CollisionData& collisionData)
{
	// Find the collision in the cache.
	IDPair ids(collisionData.firstBody->GetId(),
		collisionData.secondBody->GetId());
	auto it = (m_enableCaching ? m_collisions.find(ids) : m_collisions.end());
	
	if (it != m_collisions.end())
	{
		// Remove any contacts that have become invalidated.
		//RemoveInvalidatedContacts(&it->second);

		// Add any new contacts to the cached manifold.
		for (unsigned int i = 0; i < collisionData.numContacts; ++i)
		{
			AddNewContact(&it->second, collisionData.contacts[i]);
		}

		// Limit the number of contacts in a collision.
		LimitContactCount(&it->second, m_maxContacts);

		// Push the collision updates back to the cache.
		it->second.active = true;
	}
	else
	{
		CollisionData newCollision = collisionData;

		// Limit the number of contacts in a collision.
		LimitContactCount(&newCollision, m_maxContacts);

		// Calculate internal data for the collision and its contacts.
		//newCollision.CalcInternals();

		// Push the collision updates back to the cache.
		newCollision.active = true;
		m_collisions[ids] = newCollision;
	}
}

void CollisionCache::RemoveInactiveCollisions()
{
	if (m_enableCaching)
	{
		for (auto it = m_collisions.begin(); it != m_collisions.end(); )
		{
			if (!it->second.active)
				m_collisions.erase(it++);
			else
				++it;
		}
	}
}

void CollisionCache::MarkCollisionsAsInactive()
{
	if (m_enableCaching)
	{
		auto end = m_collisions.end();
		for (auto it = m_collisions.begin(); it != end; ++it)
			it->second.active = false;
	}
	else
	{
		m_collisions.clear();
	}
}


//bool CollisionCache::GetCollision(RigidBody* bodyA, RigidBody* bodyB, CollisionData* outCollisionData)
//{
//	IDPair ids(bodyA->GetId(), bodyB->GetId());
//	auto it = m_collisions.find(ids);
//	if (it == m_collisions.end())
//		return false;
//
//	if (outCollisionData)
//	{
//		RemoveInvalidatedContacts(&it->second);
//		m_collisions[ids] = it->second;
//		*outCollisionData = it->second;
//	}
//
//	return true;
//}
//
//void CollisionCache::AddCollision(const CollisionData& collision)
//{
//	IDPair ids(collision.firstBody->GetId(), collision.firstBody->GetId());
//	m_collisions[ids] = collision;
//}


void CollisionCache::RemoveInvalidatedContacts(CollisionData* collision)
{
	for (unsigned int i = 0; i < collision->numContacts; ++i)
	{
		Contact* contact = collision->contacts + i;

		Vector3f localToGlobalA = contact->bodyA->GetBodyToWorld().TransformAffine(contact->localPositionA);
		Vector3f localToGlobalB = contact->bodyB->GetBodyToWorld().TransformAffine(contact->localPositionB);
 
		Vector3f rAB = localToGlobalB - localToGlobalA;
		Vector3f rA = contact->worldPositionA - localToGlobalA;
		Vector3f rB = contact->worldPositionB - localToGlobalB;
 
		//contact->penetration = -contact->contactNormal.Dot(rAB);
		//bool stillPenetrating = (contact->contactNormal.Dot(rAB) <= 0.0f);
		contact->penetration += contact->contactNormal.Dot(rAB);
		bool stillPenetrating = (contact->penetration >= 0.0f);
		//contact->penetration += contact->contactNormal.Dot(rAB);
	
		bool rACloseEnough = (rA.LengthSquared() < m_persistentThresholdSqr);
		bool rBCloseEnough = (rB.LengthSquared() < m_persistentThresholdSqr);
 
		// Keep contact point if the collision pair is 
		// still colliding at this point, and the local 
		// positions are not too far from the global 
		// positions original acquired from collision detection
		if (rACloseEnough && rBCloseEnough && stillPenetrating)
		{
			// Contact is persistent: keep it.
			contact->persistent = true;
		}
		else
		{
			// Contact is invalid: remove it.
			collision->RemoveContact(i--);
		}
	}
}

bool CollisionCache::AddNewContact(CollisionData* collision, const Contact& newContact)
{
	for (unsigned int i = 0; i < collision->numContacts; ++i)
	{
		Contact* contact = collision->contacts + i;

		Vector3f rA = newContact.worldPositionA - contact->worldPositionA;
		Vector3f rB = newContact.worldPositionB - contact->worldPositionB;
 
		bool rAFarEnough = (rA.LengthSquared() > m_persistentThresholdSqr);
		bool rBFarEnough = (rB.LengthSquared() > m_persistentThresholdSqr);
 
		// Proximity check  
		if (!rAFarEnough || !rBFarEnough)
		{
			*contact = newContact;
			return false;
		}
	}

	Contact* addedContact = collision->AddContact(newContact);
	addedContact->CalculateInternals(0.0f);
	return true;
}

static float DistToLine(const Vector3f& v0, const Vector3f& v1, const Vector3f& point)
{
	Vector3f edge = v1 - v0;
	float dot = point.Dot(edge);
	if (dot > v1.Dot(edge))
		return point.DistToSqr(v1);
	else if (dot < v0.Dot(edge))
		return point.DistToSqr(v0);
	return (v0 + (dot * edge)).DistToSqr(point);
}

static float DistToTriangle(const Vector3f& a, const Vector3f& b, const Vector3f& c, const Vector3f& point)
{
	Vector3f ab = b - a;
	Vector3f bc = c - b;
	Vector3f ca = a - c;
	Vector3f abc = ab.Cross(bc);
	Vector3f abn = ab.Cross(abc);
	Vector3f bcn = bc.Cross(abc);
	Vector3f can = ca.Cross(abc);

	if (point.Dot(abn) > a.Dot(abn))
		return DistToLine(a, b, point);
	else if (point.Dot(bcn) > b.Dot(bcn))
		return DistToLine(b, c, point);
	else if (point.Dot(can) > c.Dot(can))
		return DistToLine(c, a, point);
	else
		return 0.0f; // point is inside or on triangle.
}

void CollisionCache::LimitContactCount(
	CollisionData* collision, unsigned int maxContacts)
{
	if (collision->numContacts > maxContacts)
	{
		Contact newContacts[16];
		unsigned int bestIndex;
		unsigned int i;
		float bestDist;
		float dist;
		unsigned int newNumContacts = 0;

		// First find the deepest penetrating contact.
		bestIndex = 0;
		for (i = 1; i < collision->numContacts; ++i)
		{
			if (collision->contacts[i].penetration > 
				collision->contacts[bestIndex].penetration)
			{
				bestIndex = i;
			}
		}
		newContacts[newNumContacts++] = collision->contacts[bestIndex];
		collision->RemoveContact(bestIndex);

		// Find the contact furthest from the deepest one.
		bestIndex = 0;
		bestDist = -1.0f;
		for (i = 0; i < collision->numContacts; ++i)
		{
			dist = collision->contacts[i].worldPositionB.
				DistToSqr(newContacts[0].worldPositionB);
			if (dist > bestDist)
			{
				bestDist = dist;
				bestIndex = i;
			}
		}
		newContacts[newNumContacts++] = collision->contacts[bestIndex];
		collision->RemoveContact(bestIndex);

		// Find the contact furthest from the line between prev 2 contacts.
		bestIndex = 0;
		bestDist = -1.0f;
		for (i = 0; i < collision->numContacts; ++i)
		{
			dist = DistToLine(
				newContacts[0].worldPositionB,
				newContacts[1].worldPositionB,
				collision->contacts[i].worldPositionB);
			if (dist > bestDist)
			{
				bestDist = dist;
				bestIndex = i;
			}
		}
		newContacts[newNumContacts++] = collision->contacts[bestIndex];
		collision->RemoveContact(bestIndex);

		// Find the contact furthest from the triangle between prev 3 contacts.
		bestIndex = 0;
		bestDist = -1.0f;
		for (i = 0; i < collision->numContacts; ++i)
		{
			dist = DistToTriangle(
				newContacts[0].worldPositionB,
				newContacts[1].worldPositionB,
				newContacts[2].worldPositionB,
				collision->contacts[i].worldPositionB);
			if (dist > bestDist)
			{
				bestDist = dist;
				bestIndex = i;
			}
		}
		if (bestDist > 0.0f)
			newContacts[newNumContacts++] = collision->contacts[bestIndex];

		collision->numContacts = newNumContacts;
		memcpy(collision->contacts, newContacts,
			newNumContacts * sizeof(Contact));

		//unsigned int shiftDownAmount = collision->numContacts - maxContacts;
		//collision->numContacts = maxContacts;
		//for (unsigned int i = 0; i < maxContacts; ++i)
			//collision->contacts[i] = collision->contacts[i + shiftDownAmount];
	}
}

