#ifndef _TEST_MESHES_H_
#define _TEST_MESHES_H_

#include <cmgCore/cmg_core.h>
#include <cmgGraphics/cmg_graphics.h>
#include <physics/PrimitiveMeshes.h>


struct PolyMeshFace
{
	PolyMeshFace()
	{
	}

	PolyMeshFace(unsigned int numIndices, const unsigned int* indices)
	{
		m_normal = Vector3f::UP;
		for (unsigned int i = 0; i < numIndices; i++)
			m_indices.push_back(indices[i]);
	}


	Vector3f m_normal;
	std::vector<unsigned int> m_indices;
};

struct PolyMesh
{
	void AddVertices(unsigned int numVertices, const Vector3f* vertices)
	{
		for (unsigned int i = 0; i < numVertices; i++)
			m_vertices.push_back(vertices[i]);
	}

	void AddFace(unsigned int numIndices, const unsigned int* indices)
	{
		PolyMeshFace face(numIndices, indices);
		
		if (numIndices >= 3)
		{
			Vector3f a = m_vertices[face.m_indices[0]];
			Vector3f b = m_vertices[face.m_indices[1]];
			Vector3f c = m_vertices[face.m_indices[2]];
			face.m_normal = (c - b).Cross(a - b);
			face.m_normal.Normalize();
		}
		
		Vector3f normal = Vector3f::ZERO;
		for (unsigned int i = 0; i < face.m_indices.size() - 2; i++)
		{
			Vector3f a = m_vertices[face.m_indices[i]];
			Vector3f b = m_vertices[face.m_indices[i + 1]];
			Vector3f c = m_vertices[face.m_indices[i + 2]];
			Vector3f n = (c - b).Cross(a - b);
			normal += n;
		}
		normal *= 1.0f / face.m_indices.size();
		normal.Normalize();
		face.m_normal = normal;
		m_faces.push_back(face);
	}

	std::vector<PolyMeshFace> m_faces;
	std::vector<Vector3f> m_vertices;
};


class TestMeshes
{
public:
	static Mesh* LoadMesh(const Path& path);

private:
};



#endif // _TEST_MESHES_H_