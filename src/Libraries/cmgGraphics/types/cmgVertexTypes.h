#ifndef _CMG_VERTEX_TYPES_H_
#define _CMG_VERTEX_TYPES_H_

#include <cmgGraphics/cmgOpenGLIncludes.h>
#include <cmgCore/cmgBase.h>
#include <cmgGraphics/types/cmgColor.h>
#include <cmgMath/types/cmgVector2f.h>
#include <cmgMath/types/cmgVector3f.h>
#include <cmgMath/types/cmgMatrix4f.h>

	
//-----------------------------------------------------------------------------
// Vertex Primitives
//-----------------------------------------------------------------------------

// The format in which vertex lists or vertex element arrays are organized
// into primitives.
struct VertexPrimitiveType
{ 
	typedef int value_type;

	enum
	{
		k_points = 0,
		k_lines,
		k_line_strip,
		k_line_loop,		// a single line loop
		k_triangles,
		k_triangle_strip,
		k_triangle_fan,
		k_quads,
		k_quad_strip,
		k_polygon,			// this is a *single* polygon
		k_count
	};
};

struct PrimitiveList
{
	int m_firstIndex;
	int m_numIndices;

	PrimitiveList() {}
	PrimitiveList(int firstIndex, int indexCount) :
		m_firstIndex(firstIndex),
		m_numIndices(indexCount)
	{}
};


//-----------------------------------------------------------------------------
// Standard vertex formats for models
//-----------------------------------------------------------------------------

enum class VertexAttrib
{
	k_position		= 0,
	k_tex_coord		= 1,
	k_normal		= 2,
	k_color			= 3,
	k_bone_indices	= 4,
	k_bone_weights	= 5,
	//k_tbn_matrix	= 6,
	k_count,
};

enum class AttributeType
{
	k_int,
	k_ivec2,
	k_ivec3,
	k_ivec4,

	k_float,
	k_vec2,
	k_vec3,
	k_vec4,
};

// NOTE: Order is important here
struct VertexType
{
	enum
	{
		k_position		= 0x1, // All vertices should have a position
		k_tex_coord		= 0x2,
		k_normal		= 0x4,
		k_color			= 0x8,
		k_bone_indices	= 0x10,
		k_bone_weights	= 0x20,
		//k_tbn_matrix	= 0x40, // TODO: TBN Matrix support
	};
};

struct VertexAttributeInfo
{
	VertexAttrib name;
	AttributeType type;
	unsigned int stride; // 0 = tightly packed
	const void *data;

	// Derived from type:
	unsigned int numComponents;
	unsigned int componentSize;
	AttributeType internalType;


	VertexAttributeInfo();
	VertexAttributeInfo(VertexAttrib name, AttributeType type, const void* data, unsigned int stride = 0);
};


#define DECLARE_VERTEX_TYPE(_type) \
	enum { kVertexType = _type }


#define NUM_BONES_PER_VERTEX 4

// A standard model vertex, with position, normal, texCoord, color, and bone
// weights/indices.
struct ModelVertex
{
	DECLARE_VERTEX_TYPE(VertexType::k_position |
						VertexType::k_tex_coord |
						VertexType::k_normal |
						VertexType::k_color |
						VertexType::k_bone_weights |
						VertexType::k_bone_indices);

	Vector3f position;
	Vector2f texCoord;
	Vector3f normal;
	Vector4f color;
	int32 boneIndices[NUM_BONES_PER_VERTEX];
	float boneWeights[NUM_BONES_PER_VERTEX];
};


struct VertexPosTexNormCol
{
	DECLARE_VERTEX_TYPE(VertexType::k_position |
						VertexType::k_tex_coord |
						VertexType::k_normal|
						VertexType::k_color);

	Vector3f position;
	Vector2f texCoord;
	Vector3f normal;
	Vector4f color;
};

// A vertex with a position, texture coordinate, and normal.
struct VertexPosTexNorm
{
	DECLARE_VERTEX_TYPE(VertexType::k_position |
						VertexType::k_tex_coord |
						VertexType::k_normal);

	Vector3f position;
	Vector2f texCoord;
	Vector3f normal;

	VertexPosTexNorm()
	{}

	VertexPosTexNorm(const Vector3f& position, const Vector2f& texCoord, const Vector3f& normal) :
		position(position), texCoord(texCoord), normal(normal)
	{}
};

struct VertexPosTexCol
{
	DECLARE_VERTEX_TYPE(VertexType::k_position |
						VertexType::k_tex_coord |
						VertexType::k_color);

	Vector3f position;
	Vector2f texCoord;
	Vector4f color;
};

struct VertexPosNormCol
{
	DECLARE_VERTEX_TYPE(VertexType::k_position |
						VertexType::k_normal |
						VertexType::k_color);

	Vector3f position;
	Vector3f normal;
	Vector4f color;

	VertexPosNormCol() {}

	VertexPosNormCol(const Vector3f& position) :
		position(position)
	{}
	
	VertexPosNormCol(const Vector3f& position, const Vector3f& normal,
		const Vector4f& color) :
		position(position), normal(normal), color(color)
	{}

	VertexPosNormCol(const Vector3f& position, const Vector3f& normal,
		const Color& color) :
		position(position), normal(normal), color(color.ToVector4f())
	{}
};

// A vertex with a position and texture coordinate.
struct VertexPosTex
{
	DECLARE_VERTEX_TYPE(VertexType::k_position | VertexType::k_tex_coord);

	Vector3f	position;
	Vector2f	texCoord;
	
	VertexPosTex()
	{}
	VertexPosTex(float x, float y, float z, float u, float v) :
		position(x, y, z),
		texCoord(u, v)
	{}
	VertexPosTex(const Vector3f& position, const Vector2f& texCoord) :
		position(position),
		texCoord(texCoord)
	{}
};

struct VertexPosNorm
{
	DECLARE_VERTEX_TYPE(VertexType::k_position | VertexType::k_normal);

	Vector3f	position;
	Vector3f	normal;
	
	VertexPosNorm() {}

	VertexPosNorm(const Vector3f& position, const Vector3f& normal) :
		position(position), normal(normal)
	{}
};

struct VertexPosCol
{
	DECLARE_VERTEX_TYPE(VertexType::k_position | VertexType::k_color);

	Vector3f	position;
	Vector4f	color;

	VertexPosCol() {}

	VertexPosCol(const Vector3f& position) :
		position(position)
	{}
	
	VertexPosCol(const Vector3f& position, const Vector4f& color) :
		position(position), color(color)
	{}

	VertexPosCol(const Vector3f& position, const Color& color) :
		position(position), color(color.ToVector4f())
	{}
};

struct VertexPos
{
	DECLARE_VERTEX_TYPE(VertexType::k_position);

	Vector3f position;
	
	VertexPos(float x, float y, float z) :
		position(x, y, z)
	{}
	VertexPos(const Vector3f& position) :
		position(position)
	{}
};


//typedef VertexPos VertexP;
//typedef VertexPosTex VertexPT;
//typedef VertexPosNorm VertexPN;
//typedef VertexPosCol VertexPC;
//typedef VertexPosTexNorm VertexPTN;
//typedef VertexPosTexCol VertexPTC;
//typedef VertexPosNormCol VertexPNC;
//typedef VertexPosTexNormCol VertexPTNC;

// A vertex with a position, texture coordinate, and normal.
//struct VertexPosTexTBN
//{
//	Vector3f	position;
//	Vector2f	texCoord;
//	Matrix3f	tbn;
//};


#endif // _CMG_VERTEX_TYPES_H_