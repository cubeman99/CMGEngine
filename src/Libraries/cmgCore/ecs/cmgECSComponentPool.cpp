#include "cmgECSComponentPool.h"
#include <cmgCore/cmgAssert.h>


ECSComponentPool::ECSComponentPool(uint32 componentId)
	: m_data(nullptr)
	, m_componentId(componentId)
	, m_capacity(0)
	, m_count(0)
{
	m_componentSize = 
		BaseECSComponent::GetTypeSize(m_componentId);
	m_componentCreateFunc =
		BaseECSComponent::GetTypeCreateFunction(m_componentId);
	m_componentFreeFunc =
		BaseECSComponent::GetTypeFreeFunction(m_componentId);
}

ECSComponentPool::~ECSComponentPool()
{
	Clear();
	delete [] m_data;
	m_data = nullptr;
	m_capacity = 0;
}

uint32 ECSComponentPool::size() const
{
	return m_count;
}

BaseECSComponent * ECSComponentPool::front()
{
	CMG_ASSERT(m_count > 0);
	return (BaseECSComponent*) m_data;
}

BaseECSComponent * ECSComponentPool::back()
{
	CMG_ASSERT(m_count > 0);
	return (BaseECSComponent*) &m_data[(m_count - 1) * m_componentSize];
}

BaseECSComponent * ECSComponentPool::GetComponent(ComponentHandle handle)
{
	CMG_ASSERT((uint32) handle < m_count);
	return (BaseECSComponent*) &m_data[(uint32) handle * m_componentSize];
}

void ECSComponentPool::Clear()
{
	for (uint32 i = 0; i < m_count; i++)
		m_componentFreeFunc((BaseECSComponent*) &m_data[i * m_componentSize]);
	m_count = 0;
}

ECSComponentPool::ComponentHandle ECSComponentPool::CreateComponent(const BaseECSComponent* component)
{
	if (m_capacity == 0)
		Reserve(1);
	else if (m_count + 1 > m_capacity)
		Reserve(m_capacity * 2);

	uint32 offset = m_count * m_componentSize;
	BaseECSComponent* address = (BaseECSComponent*) &m_data[offset];
	m_componentCreateFunc(address, component);
	m_count++;
	return (ComponentHandle) (m_count - 1);
}

void ECSComponentPool::DeleteComponent(ComponentHandle handle)
{
	CMG_ASSERT((uint32) handle < m_count);

	// Delete the component
	uint32 offset = (uint32) handle * m_componentSize;
	BaseECSComponent* deletedComponent = (BaseECSComponent*)
		&m_data[(uint32) handle * m_componentSize];
	m_componentFreeFunc(deletedComponent);

	m_count--;

	// Move the last component to fill the empty spot
	BaseECSComponent* lastComponent = (BaseECSComponent*)
		&m_data[m_count * m_componentSize];
	if (lastComponent != deletedComponent)
	{
		m_componentCreateFunc(deletedComponent, lastComponent);
		m_componentFreeFunc(lastComponent);
	}
}

void ECSComponentPool::Reserve(uint32 capacity)
{
	CMG_ASSERT(capacity >= m_count);
	if (capacity == m_capacity)
		return;

	uint8* oldData = m_data;
	uint8* newData = new uint8[capacity * m_componentSize];
	BaseECSComponent* oldAddress;
	BaseECSComponent* newAddress;

	for (uint32 i = 0; i < m_count; i++)
	{

		oldAddress = (BaseECSComponent*) &oldData[i * m_componentSize];
		newAddress = (BaseECSComponent*) &newData[i * m_componentSize];
		m_componentCreateFunc(newAddress, oldAddress);
		m_componentFreeFunc(oldAddress);
	}

	if (oldData != nullptr)
		delete [] oldData;
	m_data = newData;
	m_capacity = capacity;
}
