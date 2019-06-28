#ifndef _CMG_VERTEX_DATA_H_
#define _CMG_VERTEX_DATA_H_

#include <cmgGraphics/cmgOpenGLIncludes.h>
#include <cmgCore/cmg_core.h>
#include <cmgGraphics/cmgVertexTypes.h>
#include <cmgGraphics/cmgBufferObject.h>


//-----------------------------------------------------------------------------
// VertexBuffer - An object for storing a buffer of vertices interlaced a
//                particlar set of attributes.
//-----------------------------------------------------------------------------
class VertexBuffer : public BufferObject
{
	friend class Renderer;

public:
	// Constructors/destructor
	VertexBuffer();
	virtual ~VertexBuffer();
	
	// Accessors
	int GetVertexCount() const;
	inline uint32 GetGLVertexBuffer() const { return GetGLBuffer(); }
	inline uint32 GetGLVertexArray() const { return m_glVertexArray; }

	// Mutators
	void SetVertices(int numVertices, const Vector3f* vertices);
	template <class T>
	void SetVertices(int numVertices, const T* vertices);

	void BufferVertices(
		uint32 numVertices,
		const VertexAttributeInfo* attribs,
		uint32 numAttribs);

private:
	void SetVerticesRaw(uint32 vertexType, int sizeOfVertex, int numVertices, const void* vertices);

	int m_numVertices; // Number of vertices in the buffer.
	int m_bufferSize; // Size in bytes of the vertex buffer.
	uint32 m_glVertexArray; // OpenGL vertex array object ID.
	uint32 m_vertexType;
};


//-----------------------------------------------------------------------------
// IndexBuffer - An object for storing a buffer of indices that refers to the
//               indices of elements inside another buffer (usually a vertex
//               buffer).
//-----------------------------------------------------------------------------
class IndexBuffer : public BufferObject
{
	friend class Renderer;

public:
	// Constructors/destructor
	IndexBuffer();
	
	// Accessors
	uint32 GetIndexCount() const;
	inline uint32 GetGLIndexBuffer() const { return GetGLBuffer(); }
	
	// Mutators
	void SetIndices(uint32 count, const uint32* indices);
	
private:
	uint32 m_numIndices;
};


//-----------------------------------------------------------------------------
// VertexData
//-----------------------------------------------------------------------------
class VertexData
{
public:
	VertexData();
	VertexData(uint32 start, uint32 count);
	~VertexData();

	inline void BufferVertices(int numVertices, const VertexAttributeInfo* attribs, uint32 numAttribs)
	{
		m_vertexStart = 0;
		m_vertexCount = numVertices;
		m_vertexBuffer.BufferVertices(numVertices, attribs, numAttribs);
	}

	inline void BufferVertices(int numVertices, const VertexPosTex* vertices)
	{
		m_vertexStart = 0;
		m_vertexCount = numVertices;
		m_vertexBuffer.SetVertices(numVertices, vertices);
	}

	inline void BufferVertices(int numVertices, const VertexPosTexNorm* vertices)
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

	template <class T>
	void BufferVertices(const Array<T>& vertices)
	{
		m_vertexStart = 0;
		m_vertexCount = vertices.size();
		m_vertexBuffer.SetVertices((int) vertices.size(), vertices.data());
	}

	void SetVertexRange(uint32 start, uint32 count)
	{
		m_vertexStart = start;
		m_vertexCount = count;
	}

	inline void SetVertexStart(uint32 start) { m_vertexStart = start; }
	inline void SetVertexCount(uint32 count) { m_vertexCount = count; }

	inline uint32 GetStart() const { return m_vertexStart; }
	inline uint32 GetCount() const { return m_vertexCount; }
	inline VertexBuffer* GetVertexBuffer() { return &m_vertexBuffer; }


public:
	uint32 m_vertexStart;
	uint32 m_vertexCount;
	VertexBuffer m_vertexBuffer;
};


//-----------------------------------------------------------------------------
// IndexData
//-----------------------------------------------------------------------------
class IndexData
{
public:
	IndexData();
	IndexData(uint32 start, uint32 count);
	~IndexData();

	inline uint32 GetStart() const { return m_indexStart; }
	inline uint32 GetCount() const { return m_indexCount; }
	inline IndexBuffer* GetIndexBuffer() { return &m_indexBuffer; }

	inline void SetIndexStart(uint32 start) { m_indexStart = start; }
	inline void SetIndexCount(uint32 count) { m_indexCount = count; }
	void BufferIndices(const Array<uint32>& indices);		
	void BufferIndices(uint32 numIndices, const uint32* indices);
	void SetIndexRange(uint32 start, uint32 count);


public:
	uint32 m_indexStart;
	uint32 m_indexCount;
	IndexBuffer m_indexBuffer;
};



//-----------------------------------------------------------------------------
// VertexBuffer definitions
//-----------------------------------------------------------------------------

template <class T>
void VertexBuffer::SetVertices(int numVertices, const T* vertices)
{
	SetVerticesRaw(T::kVertexType, sizeof(T), numVertices, vertices);
}


#endif // _CMG_VERTEX_DATA_H_