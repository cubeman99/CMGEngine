#ifndef _CMG_BUFFER_OBJECT_H_
#define _CMG_BUFFER_OBJECT_H_

#include <cmgCore/cmgBase.h>


class BufferObject
{
public:
	BufferObject();
	virtual ~BufferObject();

	inline uint32 GetSize() const { return m_size; }
	inline uint32 GetGLTarget() const { return m_glTarget; }
	inline uint32 GetGLBuffer() const { return m_glBuffer; }

	void BufferData(const BufferObject& source);
	void BufferData(uint32 offset, const BufferObject& source);
	void BufferData(uint32 offset, uint32 size, const BufferObject& source, uint32 sourceOffset=0);
	void BufferData(uint32 size, const void* data);
	template <typename T>
	void BufferData(uint32 count, const T* data);
	void BufferSubData(uint32 offset, uint32 size, const void* data);
	template <typename T>
	void BufferSubData(uint32 index, uint32 count, const T* data);

	const void* MapBufferDataRead();
	const void* MapBufferDataRead(uint32 offset, uint32 size);
	template<typename T>
	const T* MapBufferDataRead();
	template<typename T>
	const T* MapBufferDataRead(uint32 offset, uint32 size);

	void* MapBufferDataWrite();
	void* MapBufferDataWrite(uint32 offset, uint32 size);
	template<typename T>
	T* MapBufferDataWrite();
	template<typename T>
	T* MapBufferDataWrite(uint32 offset, uint32 count);

	void UnmapBufferData();

protected:
	BufferObject(uint32 target);

private:
	uint32 m_glBuffer;
	uint32 m_glTarget;
	uint32 m_size;
};

template<typename T>
inline void BufferObject::BufferData(uint32 count, const T * data)
{
	BufferData(count * sizeof(T), (void*) data);
}

template<typename T>
inline void BufferObject::BufferSubData(uint32 index, uint32 count, const T * data)
{
	BufferSubData(index * sizeof(T), count * sizeof(T), (void*) data);
}

template<typename T>
const T * BufferObject::MapBufferDataRead()
{
	return (const T*) MapBufferDataRead(0, m_size);
}

template<typename T>
const T * BufferObject::MapBufferDataRead(uint32 index, uint32 count)
{
	return (const T*) MapBufferDataRead(index * sizeof(T), count * sizeof(T));
}

template<typename T>
T * BufferObject::MapBufferDataWrite()
{
	return (T*) MapBufferDataWrite(0, m_size);
}

template<typename T>
T * BufferObject::MapBufferDataWrite(uint32 index, uint32 count)
{
	return (T*) MapBufferDataWrite(index * sizeof(T), count * sizeof(T));
}


class ShaderStorageBuffer : public BufferObject
{
public:
	ShaderStorageBuffer();
};


class AtomicCounterBuffer : public BufferObject
{
public:
	AtomicCounterBuffer();
};


class UniformBufferObject : public BufferObject
{
public:
	UniformBufferObject();
};


#endif // _CMG_BUFFER_OBJECT_H_