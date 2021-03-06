#include "cmgVertexData.h"
#include <cmgGraphics/cmgOpenGLIncludes.h>
#include <cmgCore/cmgAssert.h>


//-----------------------------------------------------------------------------
// VertexAttributeInfo
//-----------------------------------------------------------------------------

VertexAttributeInfo::VertexAttributeInfo() :
	stride(0),
	data(nullptr),
	name(VertexAttrib::k_position),
	type(AttributeType::k_vec3),
	internalType(AttributeType::k_float),
	componentSize(sizeof(float)),
	numComponents(3)
{
}

VertexAttributeInfo::VertexAttributeInfo(VertexAttrib name, AttributeType type, const void* data, uint32 stride) :
	name(name),
	type(type),
	data(data),
	stride(stride)
{
	// Determine data type and size.
	if (type == AttributeType::k_float ||
		type == AttributeType::k_vec2 ||
		type == AttributeType::k_vec3 ||
		type == AttributeType::k_vec4)
	{
		internalType = AttributeType::k_float;
		componentSize = sizeof(float);
	}
	else if (type == AttributeType::k_int ||
		type == AttributeType::k_ivec2 ||
		type == AttributeType::k_ivec3 ||
		type == AttributeType::k_ivec4)
	{
		internalType = AttributeType::k_int;
		componentSize = sizeof(int);
	}
	else
	{
		CMG_ASSERT_FALSE("Unknown vertex attribute type");
	}
		
	// Determine number of elements.
	numComponents = (((int) type) % 4) + 1;
}


	
//-----------------------------------------------------------------------------
// VertexBuffer
//-----------------------------------------------------------------------------

VertexBuffer::VertexBuffer() :
	BufferObject(GL_ARRAY_BUFFER),
	m_numVertices(0)
{
	glGenVertexArrays(1, &m_glVertexArray);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteVertexArrays(1, &m_glVertexArray);
}

int VertexBuffer::GetVertexCount() const
{
	return m_numVertices;
}

void VertexBuffer::SetVertices(uint32 attribFlags, uint32 count, const uint8* vertices)
{
	uint32 sizeOfVertex = VertexBuffer::CalcVertexSize(attribFlags);
	SetVerticesRaw(attribFlags, sizeOfVertex, count, vertices);
}

void VertexBuffer::SetVertices(int numVertices, const Vector3f* vertices)
{
	SetVerticesRaw(VertexType::k_position, sizeof(Vector3f), numVertices, vertices);
}

void VertexBuffer::SetVerticesRaw(uint32 vertexType,
	int sizeOfVertex, int numVertices, const void* vertices)
{
	// Buffer the vertex data
	if (numVertices * sizeOfVertex > (int) GetSize())
	{
		// Buffer new vertices
		BufferData(numVertices * sizeOfVertex, vertices);
	}
	else
	{
		// Buffer over existing vertices
		BufferSubData(0, numVertices * sizeOfVertex, vertices);
	}

	// Set the attribute locations
	glBindBuffer(GL_ARRAY_BUFFER, GetGLBuffer());
	glBindVertexArray(m_glVertexArray);

	uint32 offset = 0;
	uint32 index = 0;

	m_vertexType = vertexType;

	if (m_vertexType & VertexType::k_position)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*) offset);
		offset += sizeof(Vector3f);
	}
	index++;
	if (m_vertexType & VertexType::k_tex_coord)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*) offset);
		offset += sizeof(Vector2f);
	}
	index++;
	if (m_vertexType & VertexType::k_normal)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*) offset);
		offset += sizeof(Vector3f);
	}
	index++;
	if (m_vertexType & VertexType::k_color)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*) offset);
		offset += sizeof(Vector4f);
	}
	index++;
	if (m_vertexType & VertexType::k_bone_indices)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribIPointer(index, 4, GL_INT, sizeOfVertex, (void*) offset);
		offset += sizeof(int32) * 4;
	}
	index++;
	if (m_vertexType & VertexType::k_bone_weights)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*) offset);
		offset += sizeof(Vector4f);
	}
	index++;
	// TODO: Bone and TBN attributes.

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_numVertices = numVertices;
}

void VertexBuffer::BufferVertices(
	uint32 numVertices,
	const VertexAttributeInfo* attribs,
	uint32 numAttribs)
{
	// Increase buffer size if it is too small
	uint32 newBufferSize = 0;
	for (uint32 i = 0; i < numAttribs; ++i)
	{
		newBufferSize += numVertices *
			attribs[i].numComponents * attribs[i].componentSize;
	}
	if ((int) newBufferSize > m_bufferSize)
	{
		BufferData(newBufferSize, nullptr);
		m_bufferSize = (int) newBufferSize;
	}
	
	// Setup attribute info
	glBindBuffer(GL_ARRAY_BUFFER, GetGLBuffer());
	glBindVertexArray(m_glVertexArray);
	uint32 offset = 0;
	for (uint32 i = 0; i < numAttribs; ++i)
	{
		const VertexAttributeInfo* attrib = attribs + i;
		uint32 attribIndex = (uint32) attrib->name;
		uint32 attribDataSize = numVertices *
			attrib->numComponents * attrib->componentSize;
		
		BufferSubData(offset, attribDataSize, attrib->data);
		
		glEnableVertexAttribArray(attribIndex);
		glVertexAttribPointer(attribIndex, attrib->numComponents,
			GL_FLOAT, GL_FALSE, attrib->stride, (void*) offset);
		
		offset += attribDataSize;
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_numVertices = numVertices;
}

void VertexBuffer::Clear()
{
	BufferObject::Clear();
	m_numVertices = 0;
	m_bufferSize = 0;
}

uint32 VertexBuffer::CalcVertexSize(uint32 attribFlags)
{
	uint32 size = 0;
	if (attribFlags & VertexType::k_position)
		size += sizeof(Vector3f);
	if (attribFlags & VertexType::k_tex_coord)
		size += sizeof(Vector2f);
	if (attribFlags & VertexType::k_normal)
		size += sizeof(Vector3f);
	if (attribFlags & VertexType::k_color)
		size += sizeof(Vector4f);
	if (attribFlags & VertexType::k_bone_indices)
		size += sizeof(uint32) * 4;
	if (attribFlags & VertexType::k_bone_weights)
		size += sizeof(Vector4f);
	return size;
}


//-----------------------------------------------------------------------------
// IndexBuffer
//-----------------------------------------------------------------------------

IndexBuffer::IndexBuffer() :
	BufferObject(GL_ELEMENT_ARRAY_BUFFER),
	m_numIndices(0)
{
}


void IndexBuffer::SetIndices(uint32 count, const uint32* indices)
{
	m_numIndices = count;
	BufferData(count, indices);
}

void IndexBuffer::Clear()
{
	BufferObject::Clear();
	m_numIndices = 0;
}

uint32 IndexBuffer::GetIndexCount() const
{
	return m_numIndices;
}


//-----------------------------------------------------------------------------
// VertexData
//-----------------------------------------------------------------------------

VertexData::VertexData() :
	m_vertexStart(0),
	m_vertexCount(0)
{
}

VertexData::VertexData(uint32 start, uint32 count) :
	m_vertexStart(start),
	m_vertexCount(count)
{
}

VertexData::~VertexData()
{
}

void VertexData::Clear()
{
	m_vertexBuffer.Clear();
	m_vertexCount = 0;
	m_vertexStart = 0;
}


//-----------------------------------------------------------------------------
// IndexData
//-----------------------------------------------------------------------------

IndexData::IndexData() :
	m_indexStart(0),
	m_indexCount(0)
{
}

IndexData::IndexData(uint32 start, uint32 count) :
	m_indexStart(start),
	m_indexCount(count)
{
}

IndexData::~IndexData()
{
}

void IndexData::BufferIndices(const Array<uint32>& indices)
{
	BufferIndices(indices.size(), indices.data());
}

void IndexData::BufferIndices(uint32 numIndices, const uint32* indices)
{
	m_indexStart = 0;
	m_indexCount = numIndices;
	m_indexBuffer.SetIndices(numIndices, indices);
}

void IndexData::SetIndexRange(uint32 start, uint32 count)
{
	m_indexStart = start;
	m_indexCount = count;
}

void IndexData::Clear()
{
	m_indexBuffer.Clear();
	m_indexCount = 0;
	m_indexStart = 0;
}

