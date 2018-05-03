#ifndef _CMG_PHYSICS_COLLISION_CACHE_H_
#define _CMG_PHYSICS_COLLISION_CACHE_H_

#include <map>
#include <cmgPhysics/cmgContact.h>


// http://allenchou.net/2014/01/game-physics-stability-warm-starting/


struct IDPair
{
	unsigned int idA;
	unsigned int idB;

	IDPair(unsigned int idA, unsigned int idB) :
		idA(idA),
		idB(idB)
	{
	}

	inline bool operator <(const IDPair& other) const
	{
		if (idA < other.idA)
			return true;
		else if (idA > other.idA)
			return false;
		return (idB < other.idB);
	}
};

class CollisionCache
{
	typedef std::map<IDPair, CollisionData> CollisionMap;

public:
	CollisionCache();
	~CollisionCache();

	void Clear();

	void RefreshContacts();

	void UpdateCollision(const CollisionData& collisionData);

	void RemoveInactiveCollisions();
	void MarkCollisionsAsInactive();

	//bool GetCollision(RigidBody* bodyA, RigidBody* bodyB, CollisionData* outCollisionData);
	//void AddCollision(const CollisionData& collision);

	void RemoveInvalidatedContacts(CollisionData* collision);
	bool AddNewContact(CollisionData* collision, const Contact& contact);
	void LimitContactCount(CollisionData* collision, unsigned int maxContacts);

	inline CollisionMap::iterator collisions_begin() { return m_collisions.begin(); }
	inline CollisionMap::iterator collisions_end() { return m_collisions.end(); }


private:
	bool m_enableCaching;
	CollisionMap m_collisions;
	float m_persistentThresholdSqr;
	unsigned int m_maxContacts;
};


#endif // _CMG_PHYSICS_COLLISION_CACHE_H_