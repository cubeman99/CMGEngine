#ifndef _CMG_CORE_ECS_H_
#define _CMG_CORE_ECS_H_

#include <cmgCore/ecs/cmgECSSystem.h>
#include <cmgCore/ecs/cmgECSComponentPool.h>


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
	EntityHandle CreateEntity(const T1& c1);
	template <class T1, class T2>
	EntityHandle CreateEntity(const T1& c1, const T2& c2);
	template <class T1, class T2, class T3>
	EntityHandle CreateEntity(const T1& c1, const T2& c2, const T3& c3);
	template <class T1, class T2, class T3, class T4>
	EntityHandle CreateEntity(const T1& c1, const T2& c2, const T3& c3, const T4& c4);
	template <class T1, class T2, class T3, class T4, class T5>
	EntityHandle CreateEntity(const T1& c1, const T2& c2, const T3& c3, const T4& c4, const T5& c5);
	template <class T1, class T2, class T3, class T4, class T5, class T6>
	EntityHandle CreateEntity(const T1& c1, const T2& c2, const T3& c3, const T4& c4, const T5& c5, const T6& c6);
	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	EntityHandle CreateEntity(const T1& c1, const T2& c2, const T3& c3, const T4& c4, const T5& c5, const T6& c6, const T7& c7);
	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	EntityHandle CreateEntity(const T1& c1, const T2& c2, const T3& c3, const T4& c4, const T5& c5, const T6& c6, const T7& c7, const T8& c8);
	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
	EntityHandle CreateEntity(const T1& c1, const T2& c2, const T3& c3, const T4& c4, const T5& c5, const T6& c6, const T7& c7, const T8& c8, const T9& c9);
	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
	EntityHandle CreateEntity(const T1& c1, const T2& c2, const T3& c3, const T4& c4, const T5& c5, const T6& c6, const T7& c7, const T8& c8, const T9& c9, const T10& c10);

	void ClearEntities();
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
	//void RemoveSystem(BaseECSSystem& system);

	void PrintDebug();

private:
	typedef ECSComponentPool::ComponentHandle component_handle;

	struct EntityComponent
	{
		uint32 id;
		ECSComponentPool::ComponentHandle handle;
	};

	struct Entity
	{
		uint32 index;
		Array<EntityComponent> components;
	};

	ECSComponentPool::ComponentHandle DoCreateComponent(
		EntityHandle entity, uint32 componentId,
		const BaseECSComponent* component);
	void DoRemoveComponent(uint32 componentId, component_handle handle);
	void UpdateSystemWithMultipleComponents(uint32 index, ECSSystemList& systems, float delta,
		const Array<uint32>& componentTypes, Array<BaseECSComponent*>& componentParam,
		Array<ECSComponentPool*>& componentPools);
	BaseECSComponent* GetComponentInternal(Entity& entity, ECSComponentPool* pool, uint32 componentID);
	uint32 FindLeastCommonComponent(const Array<uint32>& componentTypes, const Array<uint32>& componentFlags);

	//Array<BaseECSSystem*> m_systems;
	Map<uint32, ECSComponentPool*> m_components;
	Array<Entity*> m_entities;
};


template <class T_Component>
void ECS::AddComponent(EntityHandle entity, const T_Component& component)
{
	// Create the component then add it to the entity
	EntityComponent pair;
	pair.id = T_Component::ID;
	pair.handle = DoCreateComponent(entity, T_Component::ID, &component);
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
			DoRemoveComponent(T_Component::ID, components[i].handle);
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
			return (T_Component*) m_components[
				T_Component::ID]->GetComponent(components[i].handle);
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
	EntityHandle entity = CreateEntity();
	AddComponent(entity, component);
	return entity;
}

template <class T1, class T2>
EntityHandle ECS::CreateEntity(const T1& c1, const T2& c2)
{
	EntityHandle entity = CreateEntity();
	AddComponent(entity, c1);
	AddComponent(entity, c2);
	return entity;
}

template <class T1, class T2, class T3>
EntityHandle ECS::CreateEntity(const T1& c1, const T2& c2, const T3& c3)
{
	EntityHandle entity = CreateEntity();
	AddComponent(entity, c1);
	AddComponent(entity, c2);
	AddComponent(entity, c3);
	return entity;
}

template <class T1, class T2, class T3, class T4>
EntityHandle ECS::CreateEntity(const T1& c1, const T2& c2, const T3& c3, const T4& c4)
{
	EntityHandle entity = CreateEntity();
	AddComponent(entity, c1);
	AddComponent(entity, c2);
	AddComponent(entity, c3);
	AddComponent(entity, c4);
	return entity;
}

template <class T1, class T2, class T3, class T4, class T5>
EntityHandle ECS::CreateEntity(const T1& c1, const T2& c2, const T3& c3, const T4& c4, const T5& c5)
{
	EntityHandle entity = CreateEntity();
	AddComponent(entity, c1);
	AddComponent(entity, c2);
	AddComponent(entity, c3);
	AddComponent(entity, c4);
	AddComponent(entity, c5);
	return entity;
}

template <class T1, class T2, class T3, class T4, class T5, class T6>
EntityHandle ECS::CreateEntity(const T1& c1, const T2& c2, const T3& c3, const T4& c4, const T5& c5, const T6& c6)
{
	EntityHandle entity = CreateEntity();
	AddComponent(entity, c1);
	AddComponent(entity, c2);
	AddComponent(entity, c3);
	AddComponent(entity, c4);
	AddComponent(entity, c5);
	AddComponent(entity, c6);
	return entity;
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
EntityHandle ECS::CreateEntity(const T1& c1, const T2& c2, const T3& c3, const T4& c4, const T5& c5, const T6& c6, const T7& c7)
{
	EntityHandle entity = CreateEntity();
	AddComponent(entity, c1);
	AddComponent(entity, c2);
	AddComponent(entity, c3);
	AddComponent(entity, c4);
	AddComponent(entity, c5);
	AddComponent(entity, c6);
	AddComponent(entity, c7);
	return entity;
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
EntityHandle ECS::CreateEntity(const T1& c1, const T2& c2, const T3& c3, const T4& c4, const T5& c5, const T6& c6, const T7& c7, const T8& c8)
{
	EntityHandle entity = CreateEntity();
	AddComponent(entity, c1);
	AddComponent(entity, c2);
	AddComponent(entity, c3);
	AddComponent(entity, c4);
	AddComponent(entity, c5);
	AddComponent(entity, c6);
	AddComponent(entity, c7);
	AddComponent(entity, c8);
	return entity;
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
EntityHandle ECS::CreateEntity(const T1& c1, const T2& c2, const T3& c3, const T4& c4, const T5& c5, const T6& c6, const T7& c7, const T8& c8, const T9& c9)
{
	EntityHandle entity = CreateEntity();
	AddComponent(entity, c1);
	AddComponent(entity, c2);
	AddComponent(entity, c3);
	AddComponent(entity, c4);
	AddComponent(entity, c5);
	AddComponent(entity, c6);
	AddComponent(entity, c7);
	AddComponent(entity, c8);
	AddComponent(entity, c9);
	return entity;
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
EntityHandle ECS::CreateEntity(const T1& c1, const T2& c2, const T3& c3, const T4& c4, const T5& c5, const T6& c6, const T7& c7, const T8& c8, const T9& c9, const T10& c10)
{
	EntityHandle entity = CreateEntity();
	AddComponent(entity, c1);
	AddComponent(entity, c2);
	AddComponent(entity, c3);
	AddComponent(entity, c4);
	AddComponent(entity, c5);
	AddComponent(entity, c6);
	AddComponent(entity, c7);
	AddComponent(entity, c8);
	AddComponent(entity, c9);
	AddComponent(entity, c10);
	return entity;
}


#endif // _CMG_CORE_ECS_H_