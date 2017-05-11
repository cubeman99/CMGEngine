#include "PrimitiveMeshes.h"
#include <cmgMath/cmgMathLib.h>
#include <cmgMath/types/cmgMatrix4f.h>
#include <cmgMath/types/cmgVector2f.h>
#include <map>


typedef std::vector<VertexPosNormCol> VertexList;
typedef std::vector<unsigned int> IndexList;



static const unsigned int cubeVertexCount = 36;
static Vector3f cubePositions[cubeVertexCount];
static Vector3f cubeNormals[cubeVertexCount];
static Vector2f cubeTexCoords[cubeVertexCount];
static Vector3f cubeColors[cubeVertexCount];

static const unsigned int sphereResolutionX = 20;
static const unsigned int sphereResolutionY = 10;
static const unsigned int sphereVertexCount = (sphereResolutionX + 1) * (sphereResolutionY - 1) + (2 * sphereResolutionX);
static const unsigned int sphereIndexCount = 6 * sphereResolutionX * (sphereResolutionY - 1);
static Vector3f spherePositions[sphereVertexCount];
static Vector3f sphereNormals[sphereVertexCount];
static Vector2f sphereTexCoords[sphereVertexCount];
static Vector3f sphereColors[sphereVertexCount];
static unsigned int sphereIndices[sphereIndexCount];

static const unsigned int cylinderNumSides = 20;
static const unsigned int cylinderVertexCount = cylinderNumSides * 4;
static const unsigned int cylinderIndexCount = (cylinderNumSides * 6) + ((cylinderNumSides - 2) * 6);
static Vector3f cylinderPositions[cylinderVertexCount];
static Vector3f cylinderNormals[cylinderVertexCount];
static Vector2f cylinderTexCoords[cylinderVertexCount];
static Vector3f cylinderColors[cylinderVertexCount];
static unsigned int cylinderIndices[cylinderIndexCount];

static const unsigned int coneNumSides = 20;
static const unsigned int coneVertexCount = coneNumSides * 3;
static const unsigned int coneIndexCount = (coneNumSides * 3) + ((coneNumSides - 2) * 3);
static Vector3f conePositions[coneVertexCount];
static Vector3f coneNormals[coneVertexCount];
static Vector2f coneTexCoords[coneVertexCount];
static Vector3f coneColors[coneVertexCount];
static unsigned int coneIndices[coneIndexCount];



//-----------------------------------------------------------------------------
// Cube
//-----------------------------------------------------------------------------

void Primitives::InitializeCube()
{
	unsigned int i;

	// Setup the corner positions.
	Vector3f corner[8];
	for (i = 0; i < 8; ++i)
	{
		corner[i] = -Vector3f::ONE;
		if (i & 0x1)
			corner[i].x = 1;
		if (i & 0x2)
			corner[i].y = 1;
		if (i & 0x4)
			corner[i].z = 1;
	}

	i = 0;
	cubePositions[i++] = corner[6]; // Top
	cubePositions[i++] = corner[7];
	cubePositions[i++] = corner[3];
	cubePositions[i++] = corner[6];
	cubePositions[i++] = corner[3];
	cubePositions[i++] = corner[2];
	cubePositions[i++] = corner[5]; // Bottom
	cubePositions[i++] = corner[4];
	cubePositions[i++] = corner[0];
	cubePositions[i++] = corner[5];
	cubePositions[i++] = corner[0];
	cubePositions[i++] = corner[1];
	cubePositions[i++] = corner[5]; // Right
	cubePositions[i++] = corner[1];
	cubePositions[i++] = corner[3];
	cubePositions[i++] = corner[5];
	cubePositions[i++] = corner[3];
	cubePositions[i++] = corner[7];
	cubePositions[i++] = corner[0]; // Left
	cubePositions[i++] = corner[4];
	cubePositions[i++] = corner[6];
	cubePositions[i++] = corner[0];
	cubePositions[i++] = corner[6];
	cubePositions[i++] = corner[2];
	cubePositions[i++] = corner[1]; // Front
	cubePositions[i++] = corner[0];
	cubePositions[i++] = corner[2];
	cubePositions[i++] = corner[1];
	cubePositions[i++] = corner[2];
	cubePositions[i++] = corner[3];
	cubePositions[i++] = corner[4]; // Back
	cubePositions[i++] = corner[5];
	cubePositions[i++] = corner[7];
	cubePositions[i++] = corner[4];
	cubePositions[i++] = corner[7];
	cubePositions[i++] = corner[6];

	for (i = 0; i < 6; ++i)
	{
		cubeNormals[i + (6 * 0)] = Vector3f::UNITY;
		cubeNormals[i + (6 * 1)] = Vector3f::NEG_UNITY;
		cubeNormals[i + (6 * 2)] = Vector3f::UNITX;
		cubeNormals[i + (6 * 3)] = Vector3f::NEG_UNITX;
		cubeNormals[i + (6 * 4)] = Vector3f::NEG_UNITZ;
		cubeNormals[i + (6 * 5)] = Vector3f::UNITZ;
		
		cubeColors[i + (6 * 0)] = Vector3f(0, 1, 0);
		cubeColors[i + (6 * 1)] = Vector3f(1, 0, 1);
		cubeColors[i + (6 * 2)] = Vector3f(1, 0, 0);
		cubeColors[i + (6 * 3)] = Vector3f(0, 1, 1);
		cubeColors[i + (6 * 4)] = Vector3f(0, 0, 1);
		cubeColors[i + (6 * 5)] = Vector3f(1, 1, 0);

		cubeTexCoords[(i * 4) + 0] = Vector2f(0, 0);
		cubeTexCoords[(i * 4) + 1] = Vector2f(1, 0);
		cubeTexCoords[(i * 4) + 2] = Vector2f(1, 1);
		cubeTexCoords[(i * 4) + 3] = Vector2f(0, 0);
		cubeTexCoords[(i * 4) + 4] = Vector2f(1, 1);
		cubeTexCoords[(i * 4) + 5] = Vector2f(0, 1);
	}
}

unsigned int Primitives::GetCubeVertexCount()
{
	return cubeVertexCount;
}

const Vector3f* Primitives::GetCubePositions()
{
	return cubePositions;
}

const Vector3f* Primitives::GetCubeNormals()
{
	return cubeNormals;
}

const Vector2f* Primitives::GetCubeTexCoords()
{
	return cubeTexCoords;
}

const Vector3f* Primitives::GetCubeColors()
{
	return cubeColors;
}


//-----------------------------------------------------------------------------
// Sphere
//-----------------------------------------------------------------------------

void Primitives::InitializeSphere()
{
	Vector2f texCoord;

	Matrix3f rotationHorz = Matrix3f::CreateRotation(Vector3f::UNITY, Math::TWO_PI / sphereResolutionX);
	Matrix3f rotationVert = Matrix3f::CreateRotation(Vector3f::UNITX, Math::PI / sphereResolutionY);

	unsigned int first = sphereResolutionX * 2;

	// Create vertices at poles.
	for (unsigned int x = 0; x < sphereResolutionX; ++x)
	{
		float t = ((float) x + 0.5f) / (float) sphereResolutionX;
		sphereTexCoords[x]	= Vector2f(t, 1.0f);
		spherePositions[x]	= -Vector3f::UNITY;
		sphereNormals[x]	= -Vector3f::UNITY;
		sphereTexCoords[x + sphereResolutionX]	= Vector2f(t, 0.0f);
		spherePositions[x + sphereResolutionX]	= Vector3f::UNITY;
		sphereNormals[x + sphereResolutionX]	= Vector3f::UNITY;
	}

	// Create the first vertex.
	spherePositions[first] = rotationVert * spherePositions[0];

	// Create the first line of latitude.
	for (unsigned int y = 1; y < sphereResolutionY - 1; ++y)
	{
		spherePositions[first + y] = rotationVert *
			spherePositions[first + y - 1];
	}

	// Create the rest of the lines of latitude.
	for (unsigned int x = 1; x <= sphereResolutionX; ++x)
	{
		for (unsigned int y = 0; y < sphereResolutionY - 1; ++y)
		{
			unsigned int index = first + (x * (sphereResolutionY - 1)) + y;
			unsigned int indexPrev = index - (sphereResolutionY - 1);
			spherePositions[index] = rotationHorz * spherePositions[indexPrev];
		}
	}

	// Setup texcoords and normals.
	for (unsigned int x = 0; x <= sphereResolutionX; ++x)
	{
		texCoord.x = ((float) x / (float) sphereResolutionX);

		for (unsigned int y = 0; y < sphereResolutionY - 1; ++y)
		{
			unsigned int index = first + (x * (sphereResolutionY - 1)) + y;
			texCoord.y = 1.0f - (((float) y + 1.0f) / (float) sphereResolutionY);

			sphereTexCoords[index] = texCoord;
			sphereNormals[index] = spherePositions[index];
			sphereColors[index] = (sphereNormals[index] + Vector3f::ONE) * 0.5f;
		}
	}

	// Setup indices.
	unsigned int index = 0;
	for (unsigned int x = 0; x < sphereResolutionX; ++x)
	{
		for (unsigned int y = 0; y < sphereResolutionY - 2; ++y)
		{
			unsigned int i0 = first + (x * (sphereResolutionY - 1)) + y;
			unsigned int i1 = i0 + (sphereResolutionY - 1);
			unsigned int i2 = i0 + 1;
			unsigned int i3 = i1 + 1;

			sphereIndices[index++] = i2;
			sphereIndices[index++] = i3;
			sphereIndices[index++] = i1;
			sphereIndices[index++] = i2;
			sphereIndices[index++] = i1;
			sphereIndices[index++] = i0;

			// Poles
			if (y == 0)
			{
				sphereIndices[index++] = i0;
				sphereIndices[index++] = i1;
				sphereIndices[index++] = x;
			}
			else if (y == sphereResolutionY - 3)
			{
				sphereIndices[index++] = sphereResolutionX + x;
				sphereIndices[index++] = i2;
				sphereIndices[index++] = i3;
			}
		}
	}

	// Setup normals and colors.
	for (unsigned int i = 0; i < sphereVertexCount; ++i)
	{
		sphereNormals[i] = spherePositions[i];
		sphereColors[i] = (sphereNormals[i] + Vector3f::ONE) * 0.5f;
	}
}

unsigned int Primitives::GetSphereVertexCount()
{
	return sphereVertexCount;
}

const Vector3f* Primitives::GetSpherePositions()
{
	return spherePositions;
}

const Vector3f* Primitives::GetSphereNormals()
{
	return sphereNormals;
}

const Vector2f* Primitives::GetSphereTexCoords()
{
	return sphereTexCoords;
}

const Vector3f* Primitives::GetSphereColors()
{
	return sphereColors;
}

unsigned int Primitives::GetSphereIndexCount()
{
	return sphereIndexCount;
}

const unsigned int* Primitives::GetSphereIndices()
{
	return sphereIndices;
}


//-----------------------------------------------------------------------------
// Cylinder
//-----------------------------------------------------------------------------

void Primitives::InitializeCylinder()
{
	unsigned int index = 0;
	for (unsigned int i = 0; i < cylinderNumSides; ++i)
	{
		float angle = ((float) i / (float) cylinderNumSides) * Math::TWO_PI;
		Vector2f xy(Math::Cos(angle), Math::Sin(angle));
		cylinderPositions[i + (cylinderNumSides * 0)] = Vector3f(xy, 1.0f);
		cylinderPositions[i + (cylinderNumSides * 1)] = Vector3f(xy, -1.0f);
		cylinderPositions[i + (cylinderNumSides * 2)] = Vector3f(xy, 1.0f);
		cylinderPositions[i + (cylinderNumSides * 3)] = Vector3f(xy, -1.0f);
		cylinderNormals[i + (cylinderNumSides * 0)] = Vector3f::UNITZ;
		cylinderNormals[i + (cylinderNumSides * 1)] = -Vector3f::UNITZ;
		cylinderNormals[i + (cylinderNumSides * 2)] = Vector3f(xy, 0.0f);
		cylinderNormals[i + (cylinderNumSides * 3)] = Vector3f(xy, 0.0f);

		if (i >= 2)
		{
			cylinderIndices[index++] = 0;
			cylinderIndices[index++] = i;
			cylinderIndices[index++] = i - 1;
			cylinderIndices[index++] = cylinderNumSides + 0;
			cylinderIndices[index++] = cylinderNumSides + i - 1;
			cylinderIndices[index++] = cylinderNumSides + i;
		}
		
		cylinderIndices[index++] = (cylinderNumSides * 2) + ((i + 0) % cylinderNumSides);
		cylinderIndices[index++] = (cylinderNumSides * 2) + ((i + 1) % cylinderNumSides);
		cylinderIndices[index++] = (cylinderNumSides * 3) + ((i + 1) % cylinderNumSides);
		cylinderIndices[index++] = (cylinderNumSides * 2) + ((i + 0) % cylinderNumSides);
		cylinderIndices[index++] = (cylinderNumSides * 3) + ((i + 1) % cylinderNumSides);
		cylinderIndices[index++] = (cylinderNumSides * 3) + ((i + 0) % cylinderNumSides);
	}
}

unsigned int Primitives::GetCylinderVertexCount()
{
	return cylinderVertexCount;
}

const Vector3f* Primitives::GetCylinderPositions()
{
	return cylinderPositions;
}

const Vector3f* Primitives::GetCylinderNormals()
{
	return cylinderNormals;
}

const Vector2f* Primitives::GetCylinderTexCoords()
{
	return cylinderTexCoords;
}

const Vector3f* Primitives::GetCylinderColors()
{
	return cylinderColors;
}

unsigned int Primitives::GetCylinderIndexCount()
{
	return cylinderIndexCount;
}

const unsigned int* Primitives::GetCylinderIndices()
{
	return cylinderIndices;
}


//-----------------------------------------------------------------------------
// Cone
//-----------------------------------------------------------------------------

void Primitives::InitializeCone()
{
	unsigned int index = 0;

	float cosAngle = 1.0f / Math::Sqrt(2.0f);

	for (unsigned int i = 0; i < coneNumSides; ++i)
	{
		float angle = ((float) i / (float) coneNumSides) * Math::TWO_PI;
		float midAngle = (((float) i + 0.5f) / (float) coneNumSides) * Math::TWO_PI;
		float x = Math::Cos(angle);
		float z = Math::Sin(angle);
		float x2 = Math::Cos(midAngle);
		float z2 = Math::Sin(midAngle);
		conePositions[i + (coneNumSides * 0)] = Vector3f(x, 0.0f, z);
		conePositions[i + (coneNumSides * 1)] = Vector3f(x, 0.0f, z);
		conePositions[i + (coneNumSides * 2)] = Vector3f::UNITY;
		coneNormals[i + (coneNumSides * 0)] = -Vector3f::UNITY;
		coneNormals[i + (coneNumSides * 1)] = Vector3f(x2 * cosAngle, cosAngle, z2 * cosAngle);
		coneNormals[i + (coneNumSides * 2)] = Vector3f(x2 * cosAngle, cosAngle, z2 * cosAngle);

		if (i >= 2)
		{
			coneIndices[index++] = 0;
			coneIndices[index++] = i;
			coneIndices[index++] = i - 1;
		}
		
		coneIndices[index++] = (coneNumSides * 1) + ((i + 0) % coneNumSides);
		coneIndices[index++] = (coneNumSides * 1) + ((i + 1) % coneNumSides);
		coneIndices[index++] = (coneNumSides * 2) + ((i + 0) % coneNumSides);
	}
}

unsigned int Primitives::GetConeVertexCount()
{
	return coneVertexCount;
}

const Vector3f* Primitives::GetConePositions()
{
	return conePositions;
}

const Vector3f* Primitives::GetConeNormals()
{
	return coneNormals;
}

const Vector2f* Primitives::GetConeTexCoords()
{
	return coneTexCoords;
}

const Vector3f* Primitives::GetConeColors()
{
	return coneColors;
}

unsigned int Primitives::GetConeIndexCount()
{
	return coneIndexCount;
}

const unsigned int* Primitives::GetConeIndices()
{
	return coneIndices;
}




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

