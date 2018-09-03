#ifndef _CMG_CORE_ECS_COMPONENT_H_
#define _CMG_CORE_ECS_COMPONENT_H_

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


#endif // _CMG_CORE_ECS_COMPONENT_H_