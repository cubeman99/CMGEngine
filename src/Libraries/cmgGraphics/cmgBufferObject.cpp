#include "cmgBufferObject.h"
#include <cmgGraphics/cmgOpenGLIncludes.h>
#include <cmgCore/cmgAssert.h>


BufferObject::BufferObject() :
	m_size(0),
	m_glTarget(GL_COPY_WRITE_BUFFER)
{
	glGenBuffers(1, &m_glBuffer);
}

BufferObject::BufferObject(uint32 target) :
	m_size(0),
	m_glTarget(target)
{
	glGenBuffers(1, &m_glBuffer);
}

BufferObject::~BufferObject()
{
	glDeleteBuffers(1, &m_glBuffer);
}

void BufferObject::BufferData(const BufferObject & source)
{
	BufferData(0, source.GetSize(), source, 0);
}

void BufferObject::BufferData(uint32 offset, const BufferObject & source)
{
	BufferData(offset, source.GetSize(), source, 0);
}

void BufferObject::BufferData(uint32 offset, uint32 size, const BufferObject & source, uint32 sourceOffset)
{
	if (offset + size > m_size)
		BufferData(offset + size, nullptr);
	glBindBuffer(GL_COPY_READ_BUFFER, source.m_glBuffer);
	glBindBuffer(GL_COPY_WRITE_BUFFER, m_glBuffer);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, sourceOffset, offset, size);
}

void BufferObject::BufferData(uint32 size, const void * data)
{
	m_size = size;
	glBindBuffer(m_glTarget, m_glBuffer);
	glBufferData(m_glTarget, size, data, GL_STATIC_DRAW);
}

void BufferObject::BufferSubData(uint32 offset, uint32 size, const void * data)
{
	CMG_ASSERT(offset < m_size);
	CMG_ASSERT(offset + size <= m_size);
	glBindBuffer(m_glTarget, m_glBuffer);
	glBufferSubData(m_glTarget, offset, size, data);
}

void * BufferObject::MapBufferDataWrite()
{
	return MapBufferDataWrite(0, m_size);
}

void * BufferObject::MapBufferDataWrite(uint32 offset, uint32 size)
{
	glBindBuffer(m_glTarget, m_glBuffer);
	return glMapBufferRange(m_glTarget, offset, size, GL_MAP_WRITE_BIT);
}

const void * BufferObject::MapBufferDataRead() const
{
	return MapBufferDataRead(0, m_size);
}

const void * BufferObject::MapBufferDataRead(uint32 offset, uint32 size) const
{
	glBindBuffer(m_glTarget, m_glBuffer);
	return glMapBufferRange(m_glTarget, offset, size, GL_MAP_READ_BIT);
}

void BufferObject::UnmapBufferData() const
{
	glBindBuffer(m_glTarget, m_glBuffer);
	glUnmapBuffer(m_glTarget);
}

ShaderStorageBuffer::ShaderStorageBuffer():
	BufferObject(GL_SHADER_STORAGE_BUFFER)
{
}

AtomicCounterBuffer::AtomicCounterBuffer():
	BufferObject(GL_ATOMIC_COUNTER_BUFFER)
{
}

UniformBufferObject::UniformBufferObject():
	BufferObject(GL_UNIFORM_BUFFER)
{
}
