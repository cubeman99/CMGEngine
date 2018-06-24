#ifndef _CMG_CORE_ECS_H_
#define _CMG_CORE_ECS_H_

#include <cmgCore/containers/cmgArray.h>
#include <tuple>

struct BaseECSComponent;
typedef void* EntityHandle;
typedef uint32(*ECSComponentCreateFunction)(Array<uint8>& memory,
	EntityHandle entity, const BaseECSComponent* component);
typedef void(*ECSComponentFreeFunction)(BaseECSComponent* component);
#define NULL_ENTITY_HANDLE nullptr


struct BaseECSComponent
{
public:
	EntityHandle entity = NULL_ENTITY_HANDLE;

	virtual ~BaseECSComponent()
	{
	}

public:
	static uint32 RegisterComponentType(
		ECSComponentCreateFunction createFunction,
		ECSComponentFreeFunction freeFunction, size_t size);

	inline static ECSComponentCreateFunction GetTypeCreateFunction(uint32 id)
	{
		return GetComponentTypes()[id].createFunction;
	}

	inline static ECSComponentFreeFunction GetTypeFreeFunction(uint32 id)
	{
		return GetComponentTypes()[id].freeFunction;
	}

	inline static size_t GetTypeSize(uint32 id)
	{
		return GetComponentTypes()[id].size;
	}

	inline static bool IsTypeValid(uint32 id)
	{
		return (id < GetComponentTypes().size());
	}

private:

	struct ComponentTypeInfo
	{
		ECSComponentCreateFunction createFunction;
		ECSComponentFreeFunction freeFunction;
		size_t size;
	};

	static Array<BaseECSComponent::ComponentTypeInfo>& GetComponentTypes();

	static Array<ComponentTypeInfo>* componentTypes;
};


template <typename T_Component>
struct ECSComponent : public BaseECSComponent
{
public:
	ECSComponent();

	static const ECSComponentCreateFunction CREATE_FUNCTION;
	static const ECSComponentFreeFunction FREE_FUNCTION;
	static const uint32 ID;
	static const size_t SIZE;
};


template <typename T_Component>
uint32 ECSComponentCreate(Array<uint8>& memory,
	EntityHandle entity, const BaseECSComponent* component)
{
	// Reserve memeory for the component then invoke the copy constructor
	uint32 index = memory.size();
	memory.resize(index + T_Component::SIZE);
	T_Component* newComponent = new (&memory[index])
		T_Component(*(T_Component*) component);
	newComponent->entity = entity;
	return index;
}

template <typename T_Component>
void ECSComponentFree(BaseECSComponent* component)
{
	// Call the destructor
	T_Component* actualComponent = (T_Component*) component;
	actualComponent->~T_Component();
}


template <typename T>
ECSComponent<T>::ECSComponent()
{
}

template <typename T>
const uint32 ECSComponent<T>::ID(BaseECSComponent::RegisterComponentType(
	ECSComponentCreate<T>, ECSComponentFree<T>, sizeof(T)));

template <typename T>
const size_t ECSComponent<T>::SIZE(sizeof(T));

template <typename T>
const ECSComponentCreateFunction ECSComponent<T>::CREATE_FUNCTION(
	ECSComponentCreate<T>);

template <typename T>
const ECSComponentFreeFunction ECSComponent<T>::FREE_FUNCTION(
	ECSComponentFree<T>);


class BaseECSSystem
{
public:
	BaseECSSystem(const Array<uint32>& componentTypesIn)
		: m_componentTypes(componentTypesIn)
	{
	}

	virtual void UpdateComponents(float delta, BaseECSComponent** components, size_t numComponents)
	{
	}

	inline const Array<uint32>& GetComponentTypes()
	{
		return m_componentTypes;
	}

private:
	Array<uint32> m_componentTypes;
};


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
	inline void AddSystem(BaseECSSystem& system)
	{
		m_systems.push_back(&system);
	}
	void UpdateSystems(float deltaTime);
	void RemoveSystem(BaseECSSystem& system);

private:
	uint32 DoCreateComponent(EntityHandle entity, uint32 componentId,
		const BaseECSComponent* component);
	void DoRemoveComponent(uint32 componentId, uint32 dataOffset);

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

	Array<BaseECSSystem*> m_systems;
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