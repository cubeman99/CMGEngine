#ifndef _CMG_GRAPHICS_MESH_H_
#define _CMG_GRAPHICS_MESH_H_

#include <cmgCore/smart_ptr/cmg_smart_ptr.h>
#include <cmgGraphics/cmgVertexData.h>


struct MeshLoadOptions
{
	enum
	{
		k_none = 0,
		k_flip_triangles = 1,
	};

	typedef uint32 value_type;
};


class Mesh
{
public:
	using sptr = cmg::shared_ptr<Mesh>;

public:
	// Constructors & Destructor
	Mesh();
	~Mesh();
	
	// Accessors
	inline int GetFirstIndex() const { return m_firstIndex; }
	inline int GetNumIndices() const { return m_numIndices; }
	//inline const Bounds& GetBounds() const { return m_bounds; }
	inline VertexPrimitiveType::value_type GetPrimitiveType() const { return m_primitiveType; }
		
	inline VertexData* GetVertexData() { return &m_vertexData; }
	inline IndexData* GetIndexData() { return &m_indexData; }

	// Mutators
	inline void SetIndices(int firstIndex, int numIndices) { m_firstIndex = firstIndex; m_numIndices = numIndices; }
	//inline void SetBounds(const Bounds& bounds) { m_bounds =  bounds; }
	inline void SetPrimitiveType(VertexPrimitiveType::value_type type) { m_primitiveType = type; }

	// Static methods
	static Error Load(const Path& path, Mesh*& mesh, MeshLoadOptions::value_type options = MeshLoadOptions::k_none);

private:
	// Prevent copying
	Mesh(Mesh& other) {}
	void operator=(Mesh& other) {}


private:
	//Bounds m_bounds; // The axis-aligned bounding box of the submesh's vertices,
	int m_firstIndex; // Index to the first index of this sub-mesh in the mesh's index buffer.
	int m_numIndices; // Number of indices for this sub-mesh.

	VertexData m_vertexData;
	IndexData m_indexData;

	VertexPrimitiveType::value_type m_primitiveType; // How the index buffer is organized into primitives.
};



#endif // _CMG_GRAPHICS_MESH_H_