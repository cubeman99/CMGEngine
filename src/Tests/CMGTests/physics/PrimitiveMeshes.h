#ifndef _PRIMITIVE_MESHES_H_
#define _PRIMITIVE_MESHES_H_

#include <vector>
#include <cmgMath/types/cmgVector3f.h>
#include <cmgMath/types/cmgRay.h>
#include <cmgGraphics/cmgVertexData.h>


class Mesh
{
private:
	public:
		// Constructors/destructor.
		Mesh() :
			m_firstIndex(-1),
			m_numIndices(0),
			m_primitiveType(VertexPrimitiveType::k_triangles)
		{}
		~Mesh()
		{}
	
		// Accessors.
		inline int					GetFirstIndex()		const { return m_firstIndex; }
		inline int					GetNumIndices()		const { return m_numIndices; }
		//inline const Bounds&		GetBounds()			const { return m_bounds; }
		inline VertexPrimitiveType::value_type	GetPrimitiveType()	const { return m_primitiveType; }
		
		inline VertexData*	GetVertexData()	{ return &m_vertexData; }
		inline IndexData*	GetIndexData()	{ return &m_indexData; }

		// Mutators.
		inline void SetIndices(int firstIndex, int numIndices)	{ m_firstIndex = firstIndex; m_numIndices = numIndices; }
		//inline void SetBounds(const Bounds& bounds)				{ m_bounds =  bounds; }
		inline void SetPrimitiveType(VertexPrimitiveType::value_type type)	{ m_primitiveType = type; }

	private:
		// Prevent copying.
		Mesh(Mesh& other);
		void operator=(Mesh& other) {}

		//Bounds	m_bounds;		// The axis-aligned bounding box of the submesh's vertices,
		int			m_firstIndex;	// Index to the first index of this sub-mesh in the mesh's index buffer.
		int			m_numIndices;	// Number of indices for this sub-mesh.

		VertexData	m_vertexData;
		IndexData	m_indexData;

		VertexPrimitiveType::value_type m_primitiveType; // How the index buffer is organized into primitives.
};



class Primitives
{
public:

	static Mesh* CreateCube();
	static Mesh* CreateIcosahedron(float radoius, bool smooth = true);

	//static void CreateUVSphere(float radius, unsigned int rings, unsigned int segments);
	static Mesh* CreateIcoSphere(float radius, unsigned int subdivisions, bool smooth = true);
	static Mesh* CreateHexaSphere(float radius, unsigned int subdivisions, bool smooth = true);
	static Mesh* CreateQuadSphere(float radius, unsigned int subdivisions, bool smooth = true);
	
	static Mesh* CreateCylinder(float radius, float height, unsigned int numEdges, const Vector3f& axis, bool smooth = true);

private:
	Primitives() {}
};


#endif // _PRIMITIVE_MESHES_H_