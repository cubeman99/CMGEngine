#ifndef _CMG_PHYSICS_GJK_H_
#define _CMG_PHYSICS_GJK_H_

#include <cmgPhysics/cmgPhysicsPrimitives.h>
#include <cmgPhysics/colliders/cmgCollider.h>


class Simplex
{
	friend class GJK;

public:
	Simplex();

	void Clear();
	void Add(const Vector3f& point);
	void Remove(int index);

	inline const Vector3f& GetPoint(unsigned int index) const { return m_points[index]; }
	inline unsigned int GetNumPoints() const { return m_numPoints; }

private:
	unsigned int m_numPoints;
	Vector3f m_points[4];
};

class GJK
{
public:
	
	static bool TestIntersection(
		Collider* shapeA,
		Collider* shapeB,
		Simplex* outSimplex = nullptr);

//private:
	static Vector3f GetSupportMinkowskiDiff(const Vector3f& direction,
		Collider* shapeA, Collider* shapeB);

	static bool DoSimplex(Simplex& simplex, Vector3f& direction);
};


#endif // _CMG_PHYSICS_GJK_H_