#include "cmgECS.h"
#include "cmgAssert.h"
#include <cmgMath/cmgMathLib.h>


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
	for (uint32 i = 0; i < m_entities.size(); i++)
		delete m_entities[i];
	m_entities.clear();
}

BaseECSComponent* ECS::GetComponentInternal(Entity& entity,
	Array<uint8>& array, uint32 componentID)
{
	for (uint32 i = 0; i < entity.components.size(); i++)
	{
		if (entity.components[i].id == componentID)
		{
			return (BaseECSComponent*) &array[entity.components[i].offset];
		}
	}
	return nullptr;
}

void ECS::UpdateSystems(ECSSystemList& systems, float deltaTime)
{
	Array<BaseECSComponent*> componentParam;
	Array<Array<uint8>*> componentArrays;
	for (uint32 i = 0; i < systems.size(); i++)
	{
		const Array<uint32>& componentTypes = systems[i]->GetComponentTypes();
		if (componentTypes.size() == 1)
		{
			size_t typeSize = BaseECSComponent::GetTypeSize(componentTypes[0]);
			Array<uint8>& componentList = m_components[componentTypes[0]];
			for (uint32 j = 0; j < componentList.size(); j += typeSize)
			{
				BaseECSComponent* component = (BaseECSComponent*) &componentList[j];
				systems[i]->UpdateComponents(deltaTime, &component);
			}
		}
		else
		{
			UpdateSystemWithMultipleComponents(i, systems, deltaTime,
				componentTypes, componentParam, componentArrays);
		}
	}
}

void ECS::UpdateSystemWithMultipleComponents(uint32 index, ECSSystemList& systems, float delta,
	const Array<uint32>& componentTypes, Array<BaseECSComponent*>& componentParam,
	Array<Array<uint8>*>& componentArrays)
{
	const Array<uint32>& componentFlags = systems[index]->GetComponentFlags();

	componentParam.resize(Math::Max(componentParam.size(), componentTypes.size()));
	componentArrays.resize(Math::Max(componentArrays.size(), componentTypes.size()));
	for (uint32 i = 0; i < componentTypes.size(); i++)
	{
		componentArrays[i] = &m_components[componentTypes[i]];
	}
	uint32 minSizeIndex = FindLeastCommonComponent(componentTypes, componentFlags);

	size_t typeSize = BaseECSComponent::GetTypeSize(componentTypes[minSizeIndex]);
	Array<uint8>& array = *componentArrays[minSizeIndex];
	for (uint32 i = 0; i < array.size(); i += typeSize)
	{
		componentParam[minSizeIndex] = (BaseECSComponent*) &array[i];
		Entity* entity = (Entity*) componentParam[minSizeIndex]->entity;

		bool isValid = true;
		for (uint32 j = 0; j < componentTypes.size(); j++)
		{
			if (j == minSizeIndex)
			{
				continue;
			}

			componentParam[j] = GetComponentInternal(*entity,
				*componentArrays[j], componentTypes[j]);
			if (componentParam[j] == nullptr && (componentFlags[j] & BaseECSSystem::FLAG_OPTIONAL) == 0)
			{
				isValid = false;
				break;
			}
		}

		if (isValid)
		{
			systems[index]->UpdateComponents(delta, &componentParam[0]);
		}
	}
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

uint32 ECS::FindLeastCommonComponent(const Array<uint32>& componentTypes, const Array<uint32>& componentFlags)
{
	uint32 minSize = (uint32) -1;
	uint32 minIndex = (uint32) -1;
	for (uint32 i = 0; i < componentTypes.size(); i++)
	{
		if ((componentFlags[i] & BaseECSSystem::FLAG_OPTIONAL) != 0)
			continue;

		size_t typeSize = BaseECSComponent::GetTypeSize(componentTypes[i]);
		uint32 size = m_components[componentTypes[i]].size() / typeSize;
		if (size <= minSize)
		{
			minSize = size;
			minIndex = i;
		}
	}
	return minIndex;
}
