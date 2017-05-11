#include "cmgVertexData.h"
#include <cmgGraphics/cmgOpenGLIncludes.h>


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

VertexAttributeInfo::VertexAttributeInfo(VertexAttrib name, AttributeType type, const void* data, unsigned int stride) :
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
	m_numVertices(0),
	m_bufferSize(-1)
{
	glGenVertexArrays(1, &m_glVertexArray);
	glGenBuffers(1, &m_glVertexBuffer);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteVertexArrays(1, &m_glVertexBuffer);
	glDeleteBuffers(1, &m_glVertexBuffer);
}

int VertexBuffer::GetVertexCount() const
{
	return m_numVertices;
}

void VertexBuffer::BufferVertices(
	unsigned int numVertices,
	const VertexAttributeInfo* attribs,
	unsigned int numAttribs)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_glVertexBuffer);

	// Increase buffer size if it is too small.
	unsigned int newBufferSize = 0;
	for (unsigned int i = 0; i < numAttribs; ++i)
	{
		newBufferSize += numVertices *
			attribs[i].numComponents * attribs[i].componentSize;
	}
	if ((int) newBufferSize > m_bufferSize)
	{
		glBufferData(GL_ARRAY_BUFFER, newBufferSize, nullptr, GL_STATIC_DRAW);
		m_bufferSize = (int) newBufferSize;
	}
	
	// Setup attribute info.
	glBindVertexArray(m_glVertexArray);
	unsigned int offset = 0;
	for (unsigned int i = 0; i < numAttribs; ++i)
	{
		const VertexAttributeInfo* attrib = attribs + i;
		unsigned int attribIndex = (unsigned int) attrib->name;
		unsigned int attribDataSize = numVertices *
			attrib->numComponents * attrib->componentSize;
		
		glBufferSubData(GL_ARRAY_BUFFER, offset, attribDataSize, attrib->data);
		
		glEnableVertexAttribArray(attribIndex);
		glVertexAttribPointer(attribIndex, attrib->numComponents,
			GL_FLOAT, GL_FALSE, attrib->stride, (void*) offset);
		
		offset += attribDataSize;
	}
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_numVertices = numVertices;
}


//-----------------------------------------------------------------------------
// IndexBuffer
//-----------------------------------------------------------------------------

IndexBuffer::IndexBuffer() :
	m_numIndices(0)
{
	glGenBuffers(1, &m_glIndexBuffer);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_glIndexBuffer);
}


void IndexBuffer::SetIndices(unsigned int numIndices, const unsigned int* pIndices)
{
	m_numIndices = numIndices;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int),
		pIndices, GL_STATIC_DRAW);
}

unsigned int IndexBuffer::GetIndexCount() const
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

VertexData::VertexData(unsigned int start, unsigned int count) :
	m_vertexStart(start),
	m_vertexCount(count)
{
}

VertexData::~VertexData()
{
}


//-----------------------------------------------------------------------------
// IndexData
//-----------------------------------------------------------------------------

IndexData::IndexData() :
	m_indexStart(0),
	m_indexCount(0)
{
}

IndexData::IndexData(unsigned int start, unsigned int count) :
	m_indexStart(start),
	m_indexCount(count)
{
}

IndexData::~IndexData()
{
}

