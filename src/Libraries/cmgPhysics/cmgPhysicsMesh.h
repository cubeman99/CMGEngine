#ifndef _CMG_PHYSICS_MESH_H_
#define _CMG_PHYSICS_MESH_H_

#include <cmgMath/types/cmgVector3f.h>
#include <cmgMath/types/cmgMatrix3f.h>
#include <cmgMath/types/cmgMatrix4f.h>
#include <cmgMath/types/cmgQuaternion.h>
#include <cmgMath/types/cmgPlane.h>
#include <cmgCore/containers/cmgArray.h>
#include <cmgPhysics/cmgPhysicsPrimitives.h>


//-----------------------------------------------------------------------------
// PhysicsMesh
//-----------------------------------------------------------------------------
class PhysicsMesh
{
public:
	typedef std::vector<CollisionPrimitive*> ShapeList;

public:
	PhysicsMesh();
	PhysicsMesh(RigidBody* body);
	~PhysicsMesh();
	
	inline CollisionPrimitive* GetShape(unsigned int index) { return m_shapes[index]; }

	Vector3f GetCenterOfMass() const;
	float GetVolume() const;
	void CalcInertiaTensor(float mass, Matrix3f& outInertiaTensor) const;
	void CalcInverseInertiaTensor(float mass, Matrix3f& outInverseInertiaTensor) const;
	void CalcInertiaTensor(float mass, Matrix3f& outInertiaTensor, Matrix3f& outInverseInertiaTensor) const;
	inline ShapeList::iterator shapes_begin() { return m_shapes.begin(); }
	inline ShapeList::iterator shapes_end() { return m_shapes.end(); }

	void AddShape(CollisionPrimitive* shape);
	void ShiftOriginToCenterOfMass();

private:
	RigidBody* m_body;
	ShapeList m_shapes;
};


#endif // _CMG_PHYSICS_MESH_H_