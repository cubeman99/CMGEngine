#ifndef _CMG_CORE_ECS_COMPONENT_POOL_H_
#define _CMG_CORE_ECS_COMPONENT_POOL_H_

#include <cmgCore/ecs/cmgECSComponent.h>


class ECSComponentPool
{
public:
	using ComponentHandle = uint32;

public:
	ECSComponentPool(uint32 componentId);
	~ECSComponentPool();

	uint32 size() const;
	BaseECSComponent* front();
	BaseECSComponent* back();
	BaseECSComponent* GetComponent(ComponentHandle handle);

	void Clear();
	ComponentHandle CreateComponent(const BaseECSComponent* component);
	void DeleteComponent(ComponentHandle handle);

	
private:
	void Reserve(uint32 capacity);

	uint8* m_data;
	uint32 m_count;
	uint32 m_capacity;

	uint32 m_componentId;
	uint32 m_componentSize;
	ECSComponentCreateFunction m_componentCreateFunc;
	ECSComponentFreeFunction m_componentFreeFunc;
};


#endif // _CMG_CORE_ECS_COMPONENT_POOL_H_