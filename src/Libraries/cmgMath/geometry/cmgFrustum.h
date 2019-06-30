#ifndef _CMG_FRUSTUM_H_
#define _CMG_FRUSTUM_H_

#include <cmgMath/geometry/cmgPlane.h>

struct Bounds;
struct Quaternion;


// Frustum plane indices.
enum
{
	FRUSTUM_PLANE_RIGHT = 0,
	FRUSTUM_PLANE_LEFT,
	FRUSTUM_PLANE_TOP,
	FRUSTUM_PLANE_BOTTOM,
	FRUSTUM_PLANE_NEAR,
	FRUSTUM_PLANE_FAR,
	FRUSTUM_NUMPLANES,
};


// Frustum: a collection of 6 view planes (with normals pointing into the view area).
class Frustum
{
public:
	// Constructors.
	Frustum();

	// Accessors.
	inline const Plane& GetPlane(int index) const { return m_planes[index]; }

	// Mutators.
	Frustum& SetPlane(int index, const Vector3f& vNormal, float distance);
	Frustum& SetPlane(int index, const Plane& plane);
	void InitPerspective(const Vector3f& vOrigin, const Quaternion& qRotation,
			float fovX, float fovY, float zNear, float zFar);
	void InitPerspective(const Vector3f& vOrigin, const Vector3f& vForward,
			const Vector3f& vRight, const Vector3f& vUp,
			float fovX, float fovY, float zNear, float zFar);
	void InitOrthographic(const Vector3f& origin, const Quaternion& rotation,
			float left, float right, float bottom, float top, float zNear, float zFar);
	
	// Culling.
	bool CullBox(const Vector3f& vMin, const Vector3f& vMax) const;
	bool CullBox(const Bounds& box) const;
	bool CullBoxSkipNear(const Vector3f& vMin, const Vector3f& vMax) const;
	bool CullBoxSkipNear(const Bounds& box) const;

private:
	Plane m_planes[FRUSTUM_NUMPLANES];
};


#endif // _CMG_FRUSTUM_H_