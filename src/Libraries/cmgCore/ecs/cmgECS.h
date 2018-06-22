#ifndef _CMG_CORE_ECS_H_
#define _CMG_CORE_ECS_H_

#include <cmgCore/containers/cmgArray.h>
#include <tuple>

struct BaseECSComponent;
typedef void* EntityHandle;
typedef uint32 (*ECSComponentCreateFunction)(Array<uint8>& memory,
	EntityHandle entity, BaseECSComponent* component);
typedef void (*ECSComponentFreeFunction)(BaseECSComponent* component);
#define NULL_ENTITY_HANDLE nullptr


struct BaseECSComponent
{
public:
	virtual ~BaseECSComponent()
	{
	}

	static uint32 RegisterComponentType(
		ECSComponentCreateFunction createFunction,
		ECSComponentFreeFunction freeFunction, size_t size);

	EntityHandle entity = NULL_ENTITY_HANDLE;

	inline static ECSComponentCreateFunction GetTypeCreateFunction(uint32 id)
	{
		return componentTypes[id].createFunction;
	}

	inline static ECSComponentFreeFunction GetTypeFreeFunction(uint32 id)
	{
		return componentTypes[id].freeFunction;
	}

	inline static size_t GetTypeSize(uint32 id)
	{
		return componentTypes[id].size;
	}

	inline static bool IsTypeValid(uint32 id)
	{
		return (id < componentTypes.size());
	}

private:
	struct ComponentTypeInfo
	{
		ECSComponentCreateFunction createFunction;
		ECSComponentFreeFunction freeFunction;
		size_t size;
	};
	static Array<BaseECSComponent::ComponentTypeInfo> asdasd();
public:
	static Array<ComponentTypeInfo> componentTypes;
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
	
	static uint32 GetID();
};


template<typename T_Component>
uint32 ECSComponentCreate(Array<uint8>& memory,
	EntityHandle entity, BaseECSComponent* component)
{
	// Reserve memeory for the component then invoke the copy constructor
	uint32 index = memory.size();
	memory.resize(index + T_Component::SIZE);
	T_Component* newComponent = new (&memory[index])
		T_Component(*(T_Component*) component);
	component->entity = entity;
	return index;
}

template<typename T_Component>
void ECSComponentFree(BaseECSComponent* component)
{
	// Call the destructor
	T_Component* actualComponent = (T_Component*) component;
	actualComponent->~T_Component();
}


template<typename T>
ECSComponent<T>::ECSComponent()
{
	//static
	{
		static uint32 id = BaseECSComponent::RegisterComponentType(
			ECSComponentCreate<T>, ECSComponentFree<T>, sizeof(T));
	}
}

template<typename T>
uint32 ECSComponent<T>::GetID()
{
	static uint32 id = BaseECSComponent::RegisterComponentType(
		ECSComponentCreate<T>, ECSComponentFree<T>, sizeof(T));
	return id;
}

template<typename T>
const uint32 ECSComponent<T>::ID(0);//BaseECSComponent::RegisterComponentType(
	//ECSComponentCreate<T>, ECSComponentFree<T>, sizeof(T)));

template<typename T>
const size_t ECSComponent<T>::SIZE(sizeof(T));

template<typename T>
const ECSComponentCreateFunction ECSComponent<T>::CREATE_FUNCTION(
	ECSComponentCreate<T>);

template<typename T>
const ECSComponentFreeFunction ECSComponent<T>::FREE_FUNCTION(
	ECSComponentFree<T>);



//BEGIN EXAMPLE CODE
struct TestComponent : public ECSComponent<TestComponent>
{
	float x;
	float y;
};



//
//class Entity
//{
//public:
//	Entity();
//	~Entity();
//
//private:
//
//};


class BaseECSSystem
{
public:
	BaseECSSystem(const Array<uint32>& componentTypesIn)
		: m_componentTypes(componentTypesIn)
	{
	}

	virtual void updateComponents(float delta, BaseECSComponent** components, size_t numComponents)
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
	EntityHandle CreateEntity(BaseECSComponent* components,
		const uint32* componentIds, size_t numComponents);
	void RemoveEntity(EntityHandle handle);

	// Component methods
	template<class T_Component>
	void AddComponent(EntityHandle entity, T_Component* component);

	template<class T_Component>
	void RemoveComponent(EntityHandle entity);

	template<class T_Component>
	T_Component* GetComponent(EntityHandle entity);

	// System methods
	inline void AddSystem(BaseECSSystem& system)
	{
		m_systems.push_back(&system);
	}
	void UpdateSystems(float deltaTime);
	void RemoveSystem(BaseECSSystem& system);

private:
	struct Entity
	{
		uint32 index;
		Array<std::pair<uint32, uint32>> components;
	};

	Array<BaseECSSystem*> m_systems;
	Map<uint32, Array<uint8>> m_components;
	Array<Entity*> m_entities;
};


#endif // _CMG_CORE_ECS_H_