#ifndef _CMG_CORE_ECS_H_
#define _CMG_CORE_ECS_H_

#include <cmgCore/ecs/cmgECSSystem.h>

typedef void* EntityHandle;
#define NULL_ENTITY_HANDLE nullptr


class ECS
{
public:
	ECS();
	~ECS();


	// Entity methods
	EntityHandle CreateEntity(const BaseECSComponent* components,
		const uint32* componentIds, size_t numComponents);
	EntityHandle CreateEntity();
	template <class T1>
	EntityHandle CreateEntity(const T1& component1);
	template <class T1, class T2>
	EntityHandle CreateEntity(const T1& component1, const T2& component2);

	void RemoveEntity(EntityHandle handle);

	// Component methods
	template <class T_Component>
	void AddComponent(EntityHandle entity, const T_Component& component);

	template <class T_Component>
	void RemoveComponent(EntityHandle entity);

	template <class T_Component>
	T_Component* GetComponent(EntityHandle entity);

	template <class T_Component>
	bool HasComponent(EntityHandle entity);

	// System methods
	void UpdateSystems(ECSSystemList& systems, float deltaTime);
	void RemoveSystem(BaseECSSystem& system);

private:
	struct EntityComponent
	{
		uint32 id;
		uint32 offset;
	};

	struct Entity
	{
		uint32 index;
		Array<EntityComponent> components;
	};

	uint32 DoCreateComponent(EntityHandle entity, uint32 componentId,
		const BaseECSComponent* component);
	void DoRemoveComponent(uint32 componentId, uint32 dataOffset);
	void UpdateSystemWithMultipleComponents(uint32 index, ECSSystemList& systems, float delta,
		const Array<uint32>& componentTypes, Array<BaseECSComponent*>& componentParam,
		Array<Array<uint8>*>& componentArrays);
	BaseECSComponent* GetComponentInternal(Entity& entity, Array<uint8>& array, uint32 componentID);
	uint32 FindLeastCommonComponent(const Array<uint32>& componentTypes, const Array<uint32>& componentFlags);

	//Array<BaseECSSystem*> m_systems;
	Map<uint32, Array<uint8>> m_components;
	Array<Entity*> m_entities;
};


template <class T_Component>
void ECS::AddComponent(EntityHandle entity, const T_Component& component)
{
	// Create the component then add it to the entity
	EntityComponent pair;
	pair.id = T_Component::ID;
	pair.offset = DoCreateComponent(entity, T_Component::ID, &component);
	((Entity*) entity)->components.push_back(pair);
}

template <class T_Component>
void ECS::RemoveComponent(EntityHandle entity)
{
	Array<EntityComponent>& components = ((Entity*) entity)->components;
	for (uint32 i = 0; i < components.size(); i++)
	{
		if (components[i].id == T_Component::ID)
		{
			DoRemoveComponent(T_Component::ID, components[i].offset);
			components.erase(components.begin() + i);
			i--;
		}
	}
}

template <class T_Component>
T_Component* ECS::GetComponent(EntityHandle entity)
{
	Array<EntityComponent>& components = ((Entity*) entity)->components;
	for (uint32 i = 0; i < components.size(); i++)
	{
		if (components[i].id == T_Component::ID)
		{
			return (T_Component*) &m_components[
				T_Component::ID][components[i].offset];
		}
	}
	return nullptr;
}

template <class T_Component>
bool ECS::HasComponent(EntityHandle entity)
{
	Array<EntityComponent>& components = ((Entity*) entity)->components;
	for (uint32 i = 0; i < components.size(); i++)
	{
		if (components[i].id == T_Component::ID)
			return true;
	}
	return false;
}

template <class T1>
EntityHandle ECS::CreateEntity(const T1& component)
{
	Entity* entity = new Entity();
	entity->index = m_entities.size();
	m_entities.push_back(entity);
	AddComponent(entity, component);
	return (EntityHandle) entity;
}

template <class T1, class T2>
EntityHandle ECS::CreateEntity(const T1& component1, const T2& component2)
{
	Entity* entity = new Entity();
	entity->index = m_entities.size();
	m_entities.push_back(entity);
	AddComponent(entity, component1);
	AddComponent(entity, component2);
	return (EntityHandle) entity;
}


#endif // _CMG_CORE_ECS_H_