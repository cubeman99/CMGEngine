#include "cmgECS.h"
#include "cmgAssert.h"


Array<BaseECSComponent::ComponentTypeInfo>* BaseECSComponent::componentTypes = nullptr;



Array<BaseECSComponent::ComponentTypeInfo>& BaseECSComponent::GetComponentTypes()
{
	if (componentTypes == nullptr)
		componentTypes = new Array<ComponentTypeInfo>();
	return *componentTypes;
}

uint32 BaseECSComponent::RegisterComponentType(
	ECSComponentCreateFunction createFunction,
	ECSComponentFreeFunction freeFunction, size_t size)
{
	uint32 componentId = GetComponentTypes().size();
	ComponentTypeInfo typeInfo;
	typeInfo.createFunction = createFunction;
	typeInfo.freeFunction = freeFunction;
	typeInfo.size = size;
	GetComponentTypes().push_back(typeInfo);
	printf("Registering %d\n", GetComponentTypes().size());
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


EntityHandle ECS::CreateEntity()
{
	Entity* entity = new Entity();
	entity->index = m_entities.size();
	m_entities.push_back(entity);
	return (EntityHandle) entity;
}

EntityHandle ECS::CreateEntity(const BaseECSComponent* components,
	const uint32* componentIds, size_t numComponents)
{
	Entity* entity = (Entity*) CreateEntity();

	// Construct the components for the entity
	for (uint32 i = 0; i < numComponents; i++)
	{
		CMG_ASSERT(BaseECSComponent::IsTypeValid(componentIds[i]));

		// Create the component then add it to the entity
		EntityComponent pair;
		pair.id = componentIds[i];
		pair.offset = DoCreateComponent(entity,
			componentIds[i], components + i);
		entity->components.push_back(pair);
	}

	return (EntityHandle) entity;
}

void ECS::RemoveEntity(EntityHandle handle)
{
	Entity* entity = (Entity*) handle;

	// Delete all the entity's components from memory
	for (uint32 i = 0; i < entity->components.size(); i++)
	{
		DoRemoveComponent(entity->components[i].id, 
			entity->components[i].offset);
	}

	// Move the last entity to where the removed entity was
	uint32 destIndex = entity->index;
	uint32 srcIndex = m_entities.size() - 1;
	delete m_entities[destIndex];
	m_entities[destIndex] = m_entities[srcIndex];
	m_entities[destIndex]->index = destIndex;
	m_entities.pop_back();
}

uint32 ECS::DoCreateComponent(EntityHandle entity, uint32 componentId,
	const BaseECSComponent* component)
{
	if (m_components.find(componentId) == m_components.end())
		m_components[componentId] = Array<uint8>();
	ECSComponentCreateFunction createFunction =
		BaseECSComponent::GetTypeCreateFunction(componentId);
	uint32 offset = createFunction(
		m_components[componentId], entity, component);
	return offset;
}

void ECS::DoRemoveComponent(uint32 componentId, uint32 dataOffset)
{
	Array<uint8>& pool = m_components[componentId];
	size_t size = BaseECSComponent::GetTypeSize(componentId);

	BaseECSComponent* removedComponent =
		(BaseECSComponent*) (pool.data() + dataOffset);

	// Move the last component to where the removed component was
	size_t lastComponentOffset = pool.size() - size;
	BaseECSComponent* shiftedComponent = (BaseECSComponent*)
		(pool.data() + lastComponentOffset);
	memcpy(removedComponent, shiftedComponent, size);
	pool.resize(lastComponentOffset);
	
	// Update the reference to the shifted component's offset
	Entity* entity = (Entity*) shiftedComponent->entity;
	for (uint32 i = 0; i < entity->components.size(); i++)
	{
		if (entity->components[i].id == componentId &&
			entity->components[i].offset == lastComponentOffset)
		{
			entity->components[i].offset = dataOffset;
			return;
		}
	}
}
