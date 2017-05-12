#ifndef _CMG_PHYSICS_GJK_H_
#define _CMG_PHYSICS_GJK_H_

#include <cmgPhysics/cmgPhysicsPrimitives.h>
#include <cmgPhysics/colliders/cmgCollider.h>
#include <list>


struct SupportPoint
{
	Vector3f a; // Support point on A
	//Vector3f b; // Support point on B (redundant)
	Vector3f p; // Support point on the Minkowski Difference (A - B)

	SupportPoint* next;
};


//-----------------------------------------------------------------------------
// Simplex
//-----------------------------------------------------------------------------
class Simplex
{
	friend class GJK;
	friend struct Polytope;

public:
	Simplex();

	void Clear();
	void Add(const SupportPoint& point);
	void Remove(int index);

	inline const SupportPoint& GetPoint(unsigned int index) const { return points[index]; }
	inline unsigned int GetNumPoints() const { return numPoints; }

	unsigned int numPoints;

	union
	{
		struct { SupportPoint points[4]; };
		struct { SupportPoint a, b, c, d; };
	};
};


//-----------------------------------------------------------------------------
// Gilbert-Johnson-Keerthi
//-----------------------------------------------------------------------------
class GJK
{
public:
	
	static bool TestIntersection(
		Collider* shapeA,
		Collider* shapeB,
		Simplex* outSimplex = nullptr);

	static SupportPoint GetSupportMinkowskiDiff(
		const Vector3f& direction,
		Collider* shapeA, Collider* shapeB);

	static bool DoSimplex(Simplex& simplex, Vector3f& direction);
};


struct PolytopeFace;


struct PolytopeEdge
{
	SupportPoint* p0;
	SupportPoint* p1;

	PolytopeEdge(SupportPoint* p0, SupportPoint* p1) :
		p0(p0), p1(p1)
	{}
};

struct PolytopeFace
{
	// Faces are ordered counter-clockwise.
	SupportPoint* p0;
	SupportPoint* p1;
	SupportPoint* p2;
	Vector3f normal;
	PolytopeFace* next;
	float distToOrigin;

	PolytopeFace()
	{
	}

	PolytopeFace(SupportPoint* p0, SupportPoint* p1, SupportPoint* p2) :
		p0(p0), p1(p1), p2(p2),
		next(nullptr)
	{
		normal = (p1->p - p0->p).Cross(p2->p - p0->p).Normalize();
	}
};

struct Polytope
{
	// Linked lists.
	unsigned int m_numPoints;
	SupportPoint* m_points;
	unsigned int m_numFaces;
	PolytopeFace* m_faces;

	std::list<PolytopeEdge> m_edges;

	Polytope();
	Polytope(const Simplex& simplex);
	~Polytope();

	void AddPoint(const SupportPoint& point);
	void RemoveFace(PolytopeFace* face, PolytopeFace* facePrev);
	void AddEdge(SupportPoint* p0, SupportPoint* p1);
	PolytopeFace* FindFaceClosestOrigin();
};


struct EPAResult
{
	bool passed;
	float depth;
	Vector3f normal;
	Vector3f contactPoint;
	Vector3f contactPointA;
	Vector3f contactPointB;
};


//-----------------------------------------------------------------------------
// Expanding Polytope Algorithm
//-----------------------------------------------------------------------------
class EPA
{
public:

	static EPAResult PerformEPA(
		Collider* shapeA, Collider* shapeB,
		const Simplex& gjkTerminationSimplex);

private:
	static void Barycentric(
		const Vector3f& p, const Vector3f& a,
		const Vector3f& b, const Vector3f& c,
		float* u, float* v, float* w);
};


#endif // _CMG_PHYSICS_GJK_H_