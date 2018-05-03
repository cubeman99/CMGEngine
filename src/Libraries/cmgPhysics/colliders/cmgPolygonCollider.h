#ifndef _CMG_PHYSICS_COLLIDERS_POLYGON_COLLIDER_H_
#define _CMG_PHYSICS_COLLIDERS_POLYGON_COLLIDER_H_

#include <cmgPhysics/colliders/cmgCollider.h>
#include <vector>


class PolygonCollider : public Collider
{
public:
	PolygonCollider(unsigned int numVertices, const Vector3f* vertices);

	// Collider implementations
	float GetVolume() const override;
	Matrix3f CalcInertiaTensor(float mass) const override;
	Vector3f GetSupportPoint(const Vector3f& direction) const override;

	inline const Vector3f& GetVertex(unsigned int index) const { return m_vertices[index]; }
	inline unsigned int GetNumVertices() const { return m_vertices.size(); }
	inline std::vector<Vector3f>::iterator vertices_begin() { return m_vertices.begin(); }
	inline std::vector<Vector3f>::iterator vertices_end() { return m_vertices.end(); }
	inline std::vector<Vector3f>::const_iterator vertices_begin() const { return m_vertices.begin(); }
	inline std::vector<Vector3f>::const_iterator vertices_end() const { return m_vertices.end(); }

private:

	std::vector<Vector3f> m_vertices;
};


#endif // _CMG_PHYSICS_COLLIDERS_POLYGON_COLLIDER_H_