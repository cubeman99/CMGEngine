#ifndef _CMG_VERTEX_DATA_H_
#define _CMG_VERTEX_DATA_H_

#include <cmgGraphics/cmgOpenGLIncludes.h>
#include <cmgGraphics/types/cmgColor.h>
#include <cmgMath/types/cmgVector2f.h>
#include <cmgMath/types/cmgVector3f.h>
#include <cmgMath/types/cmgMatrix4f.h>
#include <cmgCore/cmg_core.h>

	
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

// NOTE: Order is important here
struct VertexType
{
	enum
	{
		k_position		= 0x1, // All vertices should have a position.
		k_normal		= 0x2,
		k_tex_coord		= 0x4,
		k_color			= 0x8,
		k_bone_weights	= 0x10,
		k_bone_indices	= 0x20,
		//k_tbn_matrix		= 0x40, // TODO: TBN Matrix support
	};
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

	Vector3f	position;
	Vector3f	normal;
	Vector2f	texCoord;
	Color		color;
	float		boneWeights[NUM_BONES_PER_VERTEX];
	int			boneIndices[NUM_BONES_PER_VERTEX];
};


struct VertexPosTexNormCol
{
	DECLARE_VERTEX_TYPE(VertexType::k_position |
						VertexType::k_tex_coord |
						VertexType::k_normal|
						VertexType::k_color);

	Vector3f	position;
	Vector2f	texCoord;
	Vector3f	normal;
	Vector4f	color;
};

// A vertex with a position, texture coordinate, and normal.
struct VertexPosTexNorm
{
	DECLARE_VERTEX_TYPE(VertexType::k_position |
						VertexType::k_tex_coord |
						VertexType::k_normal);

	Vector3f	position;
	Vector2f	texCoord;
	Vector3f	normal;
};

struct VertexPosTexCol
{
	DECLARE_VERTEX_TYPE(VertexType::k_position |
						VertexType::k_tex_coord |
						VertexType::k_color);

	Vector3f	position;
	Vector2f	texCoord;
	Vector4f	color;
};

struct VertexPosNormCol
{
	DECLARE_VERTEX_TYPE(VertexType::k_position |
						VertexType::k_normal |
						VertexType::k_color);

	Vector3f	position;
	Vector3f	normal;
	Vector4f	color;

	VertexPosNormCol() {}

	VertexPosNormCol(const Vector3f& position) :
		position(position)
	{}
	
	VertexPosNormCol(const Vector3f& position, const Vector3f& normal, const Vector4f& color) :
		position(position), normal(normal), color(color)
	{}

	VertexPosNormCol(const Vector3f& position, const Vector3f& normal, const Color& color) :
		position(position), normal(normal), color(color.ToVector4f())
	{}
};

// A vertex with a position and texture coordinate.
struct VertexPosTex
{
	DECLARE_VERTEX_TYPE(VertexType::k_position | VertexType::k_tex_coord);

	Vector3f	position;
	Vector2f	texCoord;
	
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


//-----------------------------------------------------------------------------
// VertexBuffer - An object for storing a buffer of vertices interlaced a
//                particlar set of attributes.
//-----------------------------------------------------------------------------
class VertexBuffer
{
	friend class Renderer;

public:
	// Constructors/destructor.
	VertexBuffer();
	~VertexBuffer();
	
	// Accessors.
	int GetVertexCount() const;
	inline unsigned int GetGLVertexBuffer() const { return m_glVertexBuffer; }
	inline unsigned int GetGLVertexArray() const { return m_glVertexArray; }

	// Mutators.
	template <class T>
	void SetVertices(int numVertices, const T* vertices);

private:
	int				m_numVertices;		// The number of vertices in the vertex buffer.
	int				m_bufferSize;		// The size in bytes of the vertex buffer.
	unsigned int	m_glVertexBuffer;	// The ID for the OpenGL vertex buffer.
	unsigned int	m_glVertexArray;	// The ID for the OpenGL vertex array object.
	unsigned int	m_vertexType;
};


//-----------------------------------------------------------------------------
// IndexBuffer - An object for storing a buffer of indices that refers to the
//               indices of elements inside another buffer (usually a vertex
//               buffer).
//-----------------------------------------------------------------------------
class IndexBuffer
{
	friend class Renderer;

public:
	// Constructors/destructor
	IndexBuffer();
	~IndexBuffer();
	
	// Accessors
	unsigned int GetIndexCount() const;
	inline unsigned int GetGLIndexBuffer() const { return m_glIndexBuffer; }
	
	// Mutators
	void SetIndices(unsigned int numIndices, const unsigned int* pIndices);
	
private:
	unsigned 		m_numIndices;
	unsigned int	m_glIndexBuffer; // The ID for the OpenGL index buffer.
};


//-----------------------------------------------------------------------------
// VertexData
//-----------------------------------------------------------------------------
class VertexData
{
public:
	VertexData();
	VertexData(unsigned int start, unsigned int count);
	~VertexData();

	void BufferVertices(int numVertices, const VertexPosTex* vertices)
	{
		m_vertexStart = 0;
		m_vertexCount = numVertices;
		m_vertexBuffer.SetVertices(numVertices, vertices);
	}

	void BufferVertices(int numVertices, const VertexPosTexNorm* vertices)
	{
		m_vertexStart = 0;
		m_vertexCount = numVertices;
		m_vertexBuffer.SetVertices(numVertices, vertices);
	}

	template <class T>
	void BufferVertices(int numVertices, const T* vertices)
	{
		m_vertexStart = 0;
		m_vertexCount = numVertices;
		m_vertexBuffer.SetVertices(numVertices, vertices);
	}

	void SetVertexRange(unsigned int start, unsigned int count)
	{
		m_vertexStart = start;
		m_vertexCount = count;
	}

	inline void SetVertexStart(unsigned int start) { m_vertexStart = start; }
	inline void SetVertexCount(unsigned int count) { m_vertexCount = count; }

	inline unsigned int GetStart() const { return m_vertexStart; }
	inline unsigned int GetCount() const { return m_vertexCount; }
	inline VertexBuffer* GetVertexBuffer() { return &m_vertexBuffer; }


public:
	unsigned int	m_vertexStart;
	unsigned int	m_vertexCount;
	VertexBuffer	m_vertexBuffer;
};


//-----------------------------------------------------------------------------
// IndexData
//-----------------------------------------------------------------------------
class IndexData
{
public:
	IndexData();
	IndexData(unsigned int start, unsigned int count);
	~IndexData();
		
	void BufferIndices(unsigned int numIndices, const unsigned int* indices)
	{
		m_indexStart = 0;
		m_indexCount = numIndices;
		m_indexBuffer.SetIndices(numIndices, indices);
	}

	void SetIndexRange(unsigned int start, unsigned int count)
	{
		m_indexStart = start;
		m_indexCount = count;
	}

	inline void SetIndexStart(unsigned int start) { m_indexStart = start; }
	inline void SetIndexCount(unsigned int count) { m_indexCount = count; }

	inline unsigned int GetStart() const { return m_indexStart; }
	inline unsigned int GetCount() const { return m_indexCount; }
	inline IndexBuffer* GetIndexBuffer() { return &m_indexBuffer; }


public:
	unsigned int	m_indexStart;
	unsigned int	m_indexCount;
	IndexBuffer		m_indexBuffer;
};



//-----------------------------------------------------------------------------
// VertexBuffer definitions
//-----------------------------------------------------------------------------

template <class T>
void VertexBuffer::SetVertices(int numVertices, const T* vertices)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_glVertexBuffer);

	// Buffer the vertex data.
	int newBufferSize = numVertices * sizeof(T);
	if (m_bufferSize < 0)
	{
		// Buffer new vertices.
		glBufferData(GL_ARRAY_BUFFER, newBufferSize, vertices, GL_STATIC_DRAW);
		m_bufferSize = newBufferSize;
	}
	else
	{
		// Buffer over existing vertices.
		CMG_ASSERT_MSG(newBufferSize <= m_bufferSize,
			"You cannot increase the buffer size"); // We mustn't increase the buffer size.
		glBufferSubData(GL_ARRAY_BUFFER, 0, newBufferSize, vertices);
	}
		
	// Set the attribute locations.
	glBindVertexArray(m_glVertexArray);

	unsigned int offset = 0;
	unsigned int index = 0;
	int sizeOfVertex = sizeof(T);

	m_vertexType = T::kVertexType;

	if (m_vertexType & VertexType::k_position)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*) offset);
		offset += sizeof(Vector3f);
		index++;
	}
	if (m_vertexType & VertexType::k_tex_coord)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*) offset);
		offset += sizeof(Vector2f);
		index++;
	}
	if (m_vertexType & VertexType::k_normal)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*) offset);
		offset += sizeof(Vector3f);
		index++;
	}
	if (m_vertexType & VertexType::k_color)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*) offset);
		offset += sizeof(Vector4f);
		index++;
	}
	// TODO: Bone and TBN attributes.

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_numVertices = numVertices;
}


#endif // _CMG_VERTEX_DATA_H_