#include "cmgFrustum.h"
#include <cmgMath/types/cmgQuaternion.h>
#include <cmgMath/geometry/cmgBounds.h>


//-----------------------------------------------------------------------------
// Constructors.
//-----------------------------------------------------------------------------

Frustum::Frustum()
{
}


//-----------------------------------------------------------------------------
// Accessors.
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

Frustum& Frustum::SetPlane(int index, const Vector3f& vNormal, float distance)
{
	m_planes[index].Set(vNormal, distance);
	return *this;
}

Frustum& Frustum::SetPlane(int index, const Plane& plane)
{
	m_planes[index] = plane;
	return *this;
}



//-----------------------------------------------------------------------------
// Frustum creation.
//-----------------------------------------------------------------------------

void Frustum::InitPerspective(const Vector3f& vOrigin,
		const Quaternion& qRotation, float fovX,
		float fovY, float zNear, float zFar)
{
	InitPerspective(vOrigin, qRotation.GetForward(),
		qRotation.GetRight(), qRotation.GetUp(),
		fovX, fovY, zNear, zFar);
}

void Frustum::InitPerspective(const Vector3f& vOrigin,
		const Vector3f& vForward, const Vector3f& vRight, const Vector3f& vUp,
		float fovX, float fovY, float zNear, float zFar)
{
	float flIntercept = Vector3f::Dot(vOrigin, vForward);

	// Setup the near and far planes.
	m_planes[FRUSTUM_PLANE_FAR].Set(-vForward, -zFar - flIntercept);
	m_planes[FRUSTUM_PLANE_NEAR].Set( vForward, zNear + flIntercept);

	Vector3f normalPos, normalNeg;
	
	// Setup the left and right planes.
	normalPos = vRight + (vForward * tanf(fovX * 0.5f));
	normalNeg = normalPos - (2.0f * vRight);
	normalPos.Normalize();
	normalNeg.Normalize();
	m_planes[FRUSTUM_PLANE_LEFT ].Set(normalPos, Vector3f::Dot(normalPos, vOrigin));
	m_planes[FRUSTUM_PLANE_RIGHT].Set(normalNeg, Vector3f::Dot(normalNeg, vOrigin));
	
	// Setup the bottom and top planes.
	normalPos = vUp + (vForward * tanf(fovY * 0.5f));
	normalNeg = normalPos - (2.0f * vUp);
	normalPos.Normalize();
	normalNeg.Normalize();
	m_planes[FRUSTUM_PLANE_BOTTOM].Set(normalPos, Vector3f::Dot(normalPos, vOrigin));
	m_planes[FRUSTUM_PLANE_TOP   ].Set(normalNeg, Vector3f::Dot(normalNeg, vOrigin));
}

void Frustum::InitOrthographic(const Vector3f& origin, const Quaternion& rotation,
		float left, float right, float bottom, float top, float zNear, float zFar)
{
	// Near and far.
	Vector3f basisForward = rotation.GetForward();
	float intercept = Vector3f::Dot(origin, basisForward);
	m_planes[FRUSTUM_PLANE_NEAR].Set(basisForward, intercept + zNear);
	m_planes[FRUSTUM_PLANE_FAR].Set(-basisForward, -(intercept + zFar));

	// Left and right.
	Vector3f basisRight = rotation.GetRight();
	intercept = Vector3f::Dot(origin, basisRight);
	m_planes[FRUSTUM_PLANE_LEFT].Set(basisRight, intercept + left);
	m_planes[FRUSTUM_PLANE_RIGHT].Set(-basisRight, -(intercept + right));
	
	// Bottom and top.
	Vector3f basisUp = rotation.GetUp();
	intercept = Vector3f::Dot(origin, basisRight);
	m_planes[FRUSTUM_PLANE_BOTTOM].Set(basisUp, intercept + bottom);
	m_planes[FRUSTUM_PLANE_TOP].Set(-basisUp, -(intercept + top));
}


//-----------------------------------------------------------------------------
// Culling.
//-----------------------------------------------------------------------------

// Returns true if the box is culled.
bool Frustum::CullBox(const Vector3f& vMin, const Vector3f& vMax) const
{
	return ((m_planes[FRUSTUM_PLANE_LEFT  ].GetBoxSide(vMin, vMax) == SIDE_BACK) || 
			(m_planes[FRUSTUM_PLANE_RIGHT ].GetBoxSide(vMin, vMax) == SIDE_BACK) || 
			(m_planes[FRUSTUM_PLANE_TOP   ].GetBoxSide(vMin, vMax) == SIDE_BACK) || 
			(m_planes[FRUSTUM_PLANE_BOTTOM].GetBoxSide(vMin, vMax) == SIDE_BACK) || 
			(m_planes[FRUSTUM_PLANE_NEAR  ].GetBoxSide(vMin, vMax) == SIDE_BACK) || 
			(m_planes[FRUSTUM_PLANE_FAR   ].GetBoxSide(vMin, vMax) == SIDE_BACK));
}

// Returns true if the box is culled (ignoring the near plane in culling calculations).
bool Frustum::CullBoxSkipNear(const Vector3f& vMin, const Vector3f& vMax) const
{
	return ((m_planes[FRUSTUM_PLANE_LEFT  ].GetBoxSide(vMin, vMax) == SIDE_BACK) || 
			(m_planes[FRUSTUM_PLANE_RIGHT ].GetBoxSide(vMin, vMax) == SIDE_BACK) || 
			(m_planes[FRUSTUM_PLANE_TOP   ].GetBoxSide(vMin, vMax) == SIDE_BACK) || 
			(m_planes[FRUSTUM_PLANE_BOTTOM].GetBoxSide(vMin, vMax) == SIDE_BACK) ||
			(m_planes[FRUSTUM_PLANE_FAR   ].GetBoxSide(vMin, vMax) == SIDE_BACK));
}

// Returns true if the box is culled.
bool Frustum::CullBox(const Bounds& box) const
{
	return CullBox(box.mins, box.maxs);
}

// Returns true if the box is culled (ignoring the near plane in culling calculations).
bool Frustum::CullBoxSkipNear(const Bounds& box) const
{
	return CullBoxSkipNear(box.mins, box.maxs);
}

