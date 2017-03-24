#include "PrimitiveMeshes.h"
#include <cmgMath/cmgMathLib.h>
#include <cmgMath/types/cmgMatrix4f.h>
#include <cmgMath/types/cmgVector2f.h>
#include <map>


typedef std::vector<VertexPosNormCol> VertexList;
typedef std::vector<unsigned int> IndexList;

static void CreateIcosahedronRaw(float radius, bool smooth, VertexList& vertices, IndexList& indices)
{
	float g = 1.61803398875f; // Golden ratio
	float l = 1.0f;
	
	// Setup initial vertices.
	vertices.push_back(Vector3f(0, -l, -g));
	vertices.push_back(Vector3f(0, l, -g));
	vertices.push_back(Vector3f(0, l, g));
	vertices.push_back(Vector3f(0, -l, g));
	vertices.push_back(Vector3f(-g, 0, -l));
	vertices.push_back(Vector3f(g, 0, -l));
	vertices.push_back(Vector3f(g, 0, l));
	vertices.push_back(Vector3f(-g, 0, l));
	vertices.push_back(Vector3f(-l, -g, 0));
	vertices.push_back(Vector3f(l, -g, 0));
	vertices.push_back(Vector3f(l, g, 0));
	vertices.push_back(Vector3f(-l, g, 0));
	
	// Setup initial indices.
	unsigned int startingIndices[60] = {
		2, 6, 3,
		2, 3, 7,
		1, 0, 5,
		1, 4, 0,
		6, 10, 5,
		6, 5, 9,
		7, 4, 11,
		7, 8, 4,
		10, 2, 11,
		10, 11, 1,
		9, 8, 3,
		9, 0, 8,
		2, 10, 6,
		2, 7, 11,
		3, 6, 9,
		3, 8, 7,
		1, 5, 10,
		1, 11, 4,
		0, 9, 5,
		0, 4, 8,
	};
	for (int i = 0; i < 60; i++)
		indices.push_back(startingIndices[i]);
	
	// Finalize vertices.
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		VertexPosNormCol& v = vertices[i];
		v.position.Normalize();
		v.normal = v.position;
		v.position *= radius;
		v.color = Vector4f((v.position + Vector3f::ONE) * 0.5f, 1);
	}

	if (!smooth)
	{
		VertexList newVertices;
		IndexList newIndices;

		for (unsigned int i = 0; i < indices.size(); i += 3)
		{
			VertexPosNormCol v1 = vertices[indices[i]];
			VertexPosNormCol v2 = vertices[indices[i + 1]];
			VertexPosNormCol v3 = vertices[indices[i + 2]];

			Vector3f normal = (v3.position - v2.position).Cross(v1.position - v2.position);
			normal.Normalize();

			v1.normal = normal;
			v2.normal = normal;
			v3.normal = normal;
			v1.color = Vector4f((normal + Vector3f::ONE) * 0.5f, 1);
			v2.color = Vector4f((normal + Vector3f::ONE) * 0.5f, 1);
			v3.color = Vector4f((normal + Vector3f::ONE) * 0.5f, 1);

			newVertices.push_back(v1);
			newVertices.push_back(v2);
			newVertices.push_back(v3);
			newIndices.push_back(i);
			newIndices.push_back(i + 1);
			newIndices.push_back(i + 2);
		}

		vertices = newVertices;
		indices = newIndices;
	}
}


Mesh* Primitives::CreateCube()
{
	float r = 1.0f;

	VertexPosNorm vertices[] = {
		VertexPosNorm(Vector3f(-r,  r, -r), Vector3f::UNITY), // Top
		VertexPosNorm(Vector3f( r,  r, -r), Vector3f::UNITY),
		VertexPosNorm(Vector3f( r,  r,  r), Vector3f::UNITY),
		VertexPosNorm(Vector3f(-r,  r,  r), Vector3f::UNITY),

		VertexPosNorm(Vector3f(-r, -r,  r), Vector3f::NEG_UNITY), // Bottom
		VertexPosNorm(Vector3f( r, -r,  r), Vector3f::NEG_UNITY),
		VertexPosNorm(Vector3f( r, -r, -r), Vector3f::NEG_UNITY),
		VertexPosNorm(Vector3f(-r, -r, -r), Vector3f::NEG_UNITY),
		
		VertexPosNorm(Vector3f( r, -r,  r), Vector3f::UNITX), // Right
		VertexPosNorm(Vector3f( r,  r,  r), Vector3f::UNITX),
		VertexPosNorm(Vector3f( r,  r, -r), Vector3f::UNITX),
		VertexPosNorm(Vector3f( r, -r, -r), Vector3f::UNITX),

		VertexPosNorm(Vector3f(-r, -r, -r), Vector3f::NEG_UNITX), // Left
		VertexPosNorm(Vector3f(-r,  r, -r), Vector3f::NEG_UNITX),
		VertexPosNorm(Vector3f(-r,  r,  r), Vector3f::NEG_UNITX),
		VertexPosNorm(Vector3f(-r, -r,  r), Vector3f::NEG_UNITX),
		
		VertexPosNorm(Vector3f( r, -r, -r), Vector3f::NEG_UNITZ), // Front
		VertexPosNorm(Vector3f( r,  r, -r), Vector3f::NEG_UNITZ),
		VertexPosNorm(Vector3f(-r,  r, -r), Vector3f::NEG_UNITZ),
		VertexPosNorm(Vector3f(-r, -r, -r), Vector3f::NEG_UNITZ),
		
		VertexPosNorm(Vector3f(-r, -r,  r), Vector3f::UNITZ), // Back
		VertexPosNorm(Vector3f(-r,  r,  r), Vector3f::UNITZ),
		VertexPosNorm(Vector3f( r,  r,  r), Vector3f::UNITZ),
		VertexPosNorm(Vector3f( r, -r,  r), Vector3f::UNITZ),
	};

	unsigned int indices[] = {
		0,1,2, 0,2,3,
		4,5,6, 4,6,7,
		8,9,10, 8,10,11,
		12,13,14, 12,14,15,
		16,17,18, 16,18,19,
		20,21,22, 20,22,23
	};

	Mesh* mesh = new Mesh();
	mesh->GetVertexData()->BufferVertices(24, vertices);
	mesh->GetIndexData()->BufferIndices(36, indices);
	return mesh;
}

Mesh* Primitives::CreateIcosahedron(float radius, bool smooth)
{
	VertexList vertices;
	IndexList indices;

	CreateIcosahedronRaw(radius, smooth, vertices, indices);

	Mesh* mesh = new Mesh();
	mesh->GetVertexData()->BufferVertices((int) vertices.size(), vertices.data());
	mesh->GetIndexData()->BufferIndices((int) indices.size(), indices.data());
	return mesh;
}

//void CreateUVSphere(float radius, unsigned int lattitude, unsigned int longitude);
//Mesh* CreateIcoSphere(float radius, unsigned int subdivisions);
//Mesh* CreateHexaSphere(float radius, unsigned int subdivisions);
//Mesh* CreateQuadSphere(float radius, unsigned int subdivisions);

typedef std::pair<unsigned int, unsigned int> EdgePair;
typedef std::map<EdgePair, unsigned int> EdgeMap;


static VertexPosNormCol SubdivideEdge(const VertexPosNormCol& a, const VertexPosNormCol& b)
{
	VertexPosNormCol ab = a;
	ab.position += b.position;
	ab.position *= 0.5f;
	ab.position.Normalize();
	return ab;
}

static void GetOrCreateEdgeVertex(EdgeMap& edgeMap, VertexList& vertices, const EdgePair& edge, unsigned int& outVertexIndex)
{
	auto it = edgeMap.find(edge);
	if (it != edgeMap.end())
	{
		outVertexIndex = it->second;
	}
	else
	{
		edgeMap[edge] = (unsigned int) vertices.size();
		edgeMap[EdgePair(edge.second, edge.first)] = (unsigned int) vertices.size();
		outVertexIndex = (unsigned int) vertices.size();

		VertexPosNormCol a = vertices[edge.first];
		VertexPosNormCol b = vertices[edge.second];
		VertexPosNormCol ab = SubdivideEdge(a, b);
		vertices.push_back(ab);
	}
}

static void FinalizeVertices(VertexList& vertices, float radius, bool smooth)
{
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		VertexPosNormCol& v = vertices[i];
		v.position.Normalize();
		if (smooth)
			v.normal = v.position;
		v.position *= radius;

		if (smooth)
		{
			v.color = Vector4f((v.normal + Vector3f::ONE) * 0.5f, 1);
		}
		else if (i % 3 == 0)
		{
			VertexPosNormCol& a = vertices[i];
			VertexPosNormCol& b = vertices[i + 1];
			VertexPosNormCol& c = vertices[i + 2];

			Vector3f normal = (c.position - b.position).Cross(a.position - b.position);
			normal.Normalize();
			Vector4f color((normal + Vector3f::ONE) * 0.5f, 1);

			a.normal = normal;
			b.normal = normal;
			c.normal = normal;
			a.color = color;
			b.color = color;
			c.color = color;
		}
	}
}

static void Subdivide(VertexList& vertices, IndexList& indices, unsigned int subdivisions, bool smooth)
{
	if (!smooth)
	{
		for (unsigned int k = 1; k < subdivisions; k++)
		{
			unsigned numVertices = vertices.size();

			std::map<EdgePair, unsigned int> edgeMap;

			for (unsigned int i = 0; i < numVertices; i += 3)
			{
				VertexPosNormCol a = vertices[i];
				VertexPosNormCol b = vertices[i + 1];
				VertexPosNormCol c = vertices[i + 2];
				VertexPosNormCol ab = SubdivideEdge(a, b);
				VertexPosNormCol bc = SubdivideEdge(b, c);
				VertexPosNormCol ca = SubdivideEdge(c, a);
				
				vertices[i] = ab;
				vertices[i + 1] = bc;
				vertices[i + 2] = ca;
				vertices.push_back(a);
				vertices.push_back(ab);
				vertices.push_back(ca);
				vertices.push_back(ab);
				vertices.push_back(b);
				vertices.push_back(bc);
				vertices.push_back(ca);
				vertices.push_back(bc);
				vertices.push_back(c);

				for (unsigned int j = 0; j < 9; j++)
					indices.push_back(indices.size());
			}
		}
	}
	else
	{
		for (unsigned int k = 1; k < subdivisions; k++)
		{
			unsigned triangleCount = indices.size() / 3;

			std::map<EdgePair, unsigned int> edgeMap;

			for (unsigned int i = 0; i < triangleCount; i++)
			{
				unsigned int indexA = i * 3;
				unsigned int indexB = indexA + 1;
				unsigned int indexC = indexB + 1;

				unsigned int vertexIndexA = indices[indexA];
				unsigned int vertexIndexB = indices[indexB];
				unsigned int vertexIndexC = indices[indexC];

				unsigned int vertexIndexAB;
				unsigned int vertexIndexBC;
				unsigned int vertexIndexAC;
				GetOrCreateEdgeVertex(edgeMap, vertices, EdgePair(vertexIndexA, vertexIndexB), vertexIndexAB);
				GetOrCreateEdgeVertex(edgeMap, vertices, EdgePair(vertexIndexB, vertexIndexC), vertexIndexBC);
				GetOrCreateEdgeVertex(edgeMap, vertices, EdgePair(vertexIndexA, vertexIndexC), vertexIndexAC);

				indices[indexA] = vertexIndexAB;
				indices[indexB] = vertexIndexBC;
				indices[indexC] = vertexIndexAC;
				indices.push_back(vertexIndexAB);
				indices.push_back(vertexIndexB);
				indices.push_back(vertexIndexBC);
				indices.push_back(vertexIndexAC);
				indices.push_back(vertexIndexBC);
				indices.push_back(vertexIndexC);
				indices.push_back(vertexIndexA);
				indices.push_back(vertexIndexAB);
				indices.push_back(vertexIndexAC);
			}
		}
	}
}

Mesh* Primitives::CreateIcoSphere(float radius, unsigned int subdivisions, bool smooth)
{
	VertexList vertices;
	IndexList indices;

	CreateIcosahedronRaw(radius, smooth, vertices, indices);
	Subdivide(vertices, indices, subdivisions, smooth);
	FinalizeVertices(vertices, radius, smooth);

	Mesh* mesh = new Mesh();
	mesh->GetVertexData()->BufferVertices((int) vertices.size(), vertices.data());
	mesh->GetIndexData()->BufferIndices((int) indices.size(), indices.data());
	return mesh;
}


Mesh* Primitives::CreateCylinder(float radius, float height, unsigned int numEdges, const Vector3f& axis, bool smooth)
{
	// Create basis vectors.
	Vector3f up = axis;
	up.Normalize();
	Vector3f right;
	if (Math::Abs(up.Dot(Vector3f::UNITX)) < 0.9f)
		right = Vector3f::UNITX;
	else
		right = Vector3f::UNITZ;
	Vector3f forward = up.Cross(right);
	forward.Normalize();
	right = forward.Cross(up);
	right.Normalize();
	
	Array<VertexPosNorm> vertices;
	Array<unsigned int> indices;
	unsigned int firstIndex = 0;
	
	// Top and bottom.
	for (unsigned int side = 0; side < 2; side++)
	{
		firstIndex = vertices.size();

		for (unsigned int i = 0; i < numEdges; i++)
		{
			float sideSign = (side == 0 ? 1.0f : -1.0f);
			float angle = (i / (float) numEdges) * Math::TWO_PI * sideSign;

			VertexPosNorm vertex;
			vertex.position = up * sideSign * (height * 0.5f);
			vertex.position += right * radius * Math::Cos(angle);
			vertex.position += forward * radius * Math::Sin(angle);
			vertex.normal = up * sideSign;

			if (i >= 2)
			{
				indices.push_back(firstIndex);
				indices.push_back(firstIndex + i - 1);
				indices.push_back(firstIndex + i);
			}

			vertices.push_back(vertex);
		}
	}

	// Sides.
	firstIndex = vertices.size();

	for (unsigned int i = 0; i < numEdges; i++)
	{
		float angle = (i / (float) numEdges) * Math::TWO_PI;

		VertexPosNorm vertex;
		vertex.position = right * radius * Math::Cos(-angle);
		vertex.position += forward * radius * Math::Sin(-angle);
		vertex.normal = vertex.position;
		vertex.normal.Normalize();

		unsigned int n = numEdges * 2;
		indices.push_back(firstIndex + ((i*2 + 0) % n));
		indices.push_back(firstIndex + ((i*2 + 1) % n));
		indices.push_back(firstIndex + ((i*2 + 3) % n));
		indices.push_back(firstIndex + ((i*2 + 0) % n));
		indices.push_back(firstIndex + ((i*2 + 3) % n));
		indices.push_back(firstIndex + ((i*2 + 2) % n));
		
		// Bottom then top.
		vertex.position -= up * (height * 0.5f);
		vertices.push_back(vertex);
		vertex.position += up * height;
		vertices.push_back(vertex);
	}

	Mesh* mesh = new Mesh();
	mesh->GetVertexData()->BufferVertices((int) vertices.size(), vertices.data());
	mesh->GetIndexData()->BufferIndices((int) indices.size(), indices.data());
	return mesh;
}
