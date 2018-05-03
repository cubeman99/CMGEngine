#ifndef _CMG_PHYSICS_COLLIDERS_CONVEX_MESH_COLLIDER_H_
#define _CMG_PHYSICS_COLLIDERS_CONVEX_MESH_COLLIDER_H_

#include <cmgPhysics/colliders/cmgCollider.h>
#include <vector>


//-----------------------------------------------------------------------------
//  Half-Edge Mesh description
//-----------------------------------------------------------------------------
// Quoted from: Dirk Gregorius (2013) - "Physics for Game Programmers:
//    The Separating Axis Test Between Convex Polyhedra"
// 
// * Edge-centric data structure
// * Makes it easy to iterate edges of a face and to access neighboring faces 
//
// * For each face we store:
//   - A half-edge that defines the entry into a circular list around the face
//
// * For each edge we store:
//   - The previous and next edge that build the circular face list around the face
//   - A twin edge to cross over to adjacent faces
//   - The tail vertex of the edge
//   - Note that we don’t need to store the head vertex since it is simple
//     the tail vertex of the twin edge
// 
//-----------------------------------------------------------------------------
//  
//      o          CCW winding order         o
//       \                                  ^
//        \                                /
//         \                              /
//          \           e.twin           /
//           v -----------------------> /
//   e.head  o <---------------------- o  e.tail
//          /        Half-Edge e        ^
//         /                             \
//        / e.next                 e.prev \
//       v                                 \
//      o               e.face              o
//     
//-----------------------------------------------------------------------------

struct ConvexMeshFace;

struct ConvexMeshVertex
{
	Vector3f position;
};

struct ConvexMeshHalfEdge
{
	ConvexMeshHalfEdge* twin;
	ConvexMeshVertex* tail;
	ConvexMeshFace* face;
	ConvexMeshHalfEdge* next;
};

struct ConvexMeshFace
{
	unsigned int numEdges;
	ConvexMeshHalfEdge* edge;
	Vector3f normal;
};


class ConvexMeshCollider : public Collider
{
public:
	ConvexMeshCollider();

	// Collider implementations
	float GetVolume() const override;
	Matrix3f CalcInertiaTensor(float mass) const override;
	Vector3f GetSupportPoint(const Vector3f& direction) const override;

	inline const ConvexMeshVertex& GetVertex(unsigned int index) const { return m_vertices[index]; }
	inline unsigned int GetNumVertices() const { return m_vertices.size(); }

	inline const ConvexMeshHalfEdge& GetEdge(unsigned int index) const { return m_edges[index]; }
	inline unsigned int GetNumEdges() const { return m_edges.size() / 2; }
	inline unsigned int GetNumHalfEdges() const { return m_edges.size(); }

	inline const ConvexMeshFace& GetFace(unsigned int index) const { return m_faces[index]; }
	inline unsigned int GetNumFaces() const { return m_faces.size(); }

private:

	std::vector<ConvexMeshVertex> m_vertices;
	std::vector<ConvexMeshHalfEdge> m_edges;
	std::vector<ConvexMeshFace> m_faces;
};


#endif // _CMG_PHYSICS_COLLIDERS_CONVEX_MESH_COLLIDER_H_