#include "cmgECS.h"
#include "cmgAssert.h"


Array<BaseECSComponent::ComponentTypeInfo> BaseECSComponent::asdasd()
{
	printf("componentTypes Constructor\n");
	return Array<BaseECSComponent::ComponentTypeInfo>();
}

Array<BaseECSComponent::ComponentTypeInfo> BaseECSComponent::componentTypes = BaseECSComponent::asdasd();


uint32 BaseECSComponent::RegisterComponentType(
	ECSComponentCreateFunction createFunction,
	ECSComponentFreeFunction freeFunction, size_t size)
{
	uint32 componentId = componentTypes.size();
	ComponentTypeInfo typeInfo;
	typeInfo.createFunction = createFunction;
	typeInfo.freeFunction = freeFunction;
	typeInfo.size = size;
	componentTypes.push_back(typeInfo);
	printf("Registering %d\n", componentTypes.size());
	return componentId;
}


ECS::ECS()
{
}

ECS::~ECS()
{
	// Delete all components
	for (auto it = m_components.begin(); it != m_components.end(); ++it)
	{
		size_t typeSize = BaseECSComponent::GetTypeSize(it->first);
		ECSComponentFreeFunction freeFunction =
			BaseECSComponent::GetTypeFreeFunction(it->first);
		for (uint32 i = 0; i < it->second.size(); i += typeSize)
		{
			freeFunction((BaseECSComponent*) &it->second[i]);
		}
	}

	// Delete all entities
	for(uint32 i = 0; i < m_entities.size(); i++)
		delete m_entities[i];
	m_entities.clear();
}

EntityHandle ECS::CreateEntity(BaseECSComponent* components,
	const uint32* componentIds, size_t numComponents)
{
	// Create th eentity
	Entity* entity = new Entity();
	entity->index = m_entities.size();
	m_entities.push_back(entity);

	// Construct the components for the entity
	for (uint32 i = 0; i < numComponents; i++)
	{
		CMG_ASSERT(BaseECSComponent::IsTypeValid(componentIds[i]));

		// Create the component then add it to the entity
		ECSComponentCreateFunction createFunction =
			BaseECSComponent::GetTypeCreateFunction(componentIds[i]);
		std::pair<uint32, uint32> newPair;
		newPair.first = componentIds[i];
		newPair.second = createFunction(m_components[componentIds[i]],
			entity, &components[i]);
		entity->components.push_back(newPair);
	}

	return (EntityHandle) entity;
}

void ECS::RemoveEntity(EntityHandle handle)
{
	Entity* entity = (Entity*) handle;

	for (uint32 i = 0; i < entity->components.size(); i++)
	{
		//removeComponentInternal(entity[i].first, entity[i].second);
	}

	// Remove the entity from the list by moving the last element
	// to the index of the removed element.
	uint32 destIndex = entity->index;
	uint32 srcIndex = m_entities.size() - 1;
	delete m_entities[destIndex];
	m_entities[destIndex] = m_entities[srcIndex];
	m_entities.pop_back();
}
