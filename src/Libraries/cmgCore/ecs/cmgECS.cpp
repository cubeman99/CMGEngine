#include "cmgECS.h"
#include "cmgAssert.h"
#include <cmgMath/cmgMathLib.h>


ECS::ECS()
{
}

ECS::~ECS()
{
	// Delete all component pools
	for (auto it = m_components.begin(); it != m_components.end(); ++it)
		delete it->second;
	m_components.clear();

	// Delete all entities
	for (uint32 i = 0; i < m_entities.size(); i++)
		delete m_entities[i];
	m_entities.clear();
}

BaseECSComponent* ECS::GetComponentInternal(Entity& entity,
	ECSComponentPool* pool, uint32 componentID)
{
	for (uint32 i = 0; i < entity.components.size(); i++)
	{
		if (entity.components[i].id == componentID)
			return pool->GetComponent(entity.components[i].handle);
	}
	return nullptr;
}

void ECS::UpdateSystems(ECSSystemList& systems, float deltaTime)
{
	Array<BaseECSComponent*> componentParam;
	Array<ECSComponentPool*> componentPools;
	for (uint32 i = 0; i < systems.Size(); i++)
	{
		systems[i]->PreUpdate(deltaTime);
	}
	for (uint32 i = 0; i < systems.Size(); i++)
	{
		const Array<uint32>& componentTypes = systems[i]->GetComponentTypes();

		// Verify each component type for this system has a pool created
		for (uint32 j = 0; j < componentTypes.size(); j++)
		{
			if (m_components.find(componentTypes[j]) == m_components.end())
				m_components[componentTypes[j]] = new ECSComponentPool(componentTypes[j]);
		}

		if (componentTypes.size() == 1)
		{
			ECSComponentPool* pool = m_components[componentTypes[0]];
			for (uint32 j = 0; j < pool->size(); j++)
			{
				BaseECSComponent* component = pool->GetComponent(j);
				systems[i]->UpdateComponents(deltaTime, &component);
			}
		}
		else
		{
			UpdateSystemWithMultipleComponents(i, systems, deltaTime,
				componentTypes, componentParam, componentPools);
		}
	}
	for (uint32 i = 0; i < systems.Size(); i++)
	{
		systems[i]->PostUpdate(deltaTime);
	}
}

void ECS::UpdateSystemWithMultipleComponents(uint32 index, ECSSystemList& systems, float delta,
	const Array<uint32>& componentTypes, Array<BaseECSComponent*>& componentParam,
	Array<ECSComponentPool*>& componentPools)
{
	const Array<uint32>& componentFlags = systems[index]->GetComponentFlags();

	componentParam.resize(Math::Max(componentParam.size(), componentTypes.size()));
	componentPools.resize(Math::Max(componentPools.size(), componentTypes.size()));
	for (uint32 i = 0; i < componentTypes.size(); i++)
		componentPools[i] = m_components[componentTypes[i]];
	uint32 minSizeIndex = FindLeastCommonComponent(componentTypes, componentFlags);

	ECSComponentPool* pool = componentPools[minSizeIndex];
	for (uint32 i = 0; i < pool->size(); i++)
	{
		componentParam[minSizeIndex] = pool->GetComponent(i);
		Entity* entity = (Entity*) componentParam[minSizeIndex]->entity;

		bool isValid = true;
		for (uint32 j = 0; j < componentTypes.size(); j++)
		{
			if (j == minSizeIndex)
			{
				continue;
			}

			componentParam[j] = GetComponentInternal(*entity,
				componentPools[j], componentTypes[j]);
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
		pair.handle = DoCreateComponent(entity,
			componentIds[i], components + i);
		entity->components.push_back(pair);
	}

	return (EntityHandle) entity;
}

void ECS::ClearEntities()
{
	while (m_entities.size() > 0)
	{
		RemoveEntity((EntityHandle) m_entities[0]);
	}
}

void ECS::RemoveEntity(EntityHandle handle)
{
	Entity* entity = (Entity*) handle;

	// Delete all the entity's components from memory
	for (uint32 i = 0; i < entity->components.size(); i++)
	{
		DoRemoveComponent(entity->components[i].id,
			entity->components[i].handle);
	}

	// Move the last entity to where the removed entity was
	uint32 destIndex = entity->index;
	uint32 srcIndex = m_entities.size() - 1;
	delete m_entities[destIndex];
	m_entities[destIndex] = m_entities[srcIndex];
	m_entities[destIndex]->index = destIndex;
	m_entities.pop_back();
}

ECSComponentPool::ComponentHandle ECS::DoCreateComponent(
	EntityHandle entity, uint32 componentId,
	const BaseECSComponent* component)
{
	if (m_components.find(componentId) == m_components.end())
		m_components[componentId] = new ECSComponentPool(componentId);
	ECSComponentPool* pool = m_components[componentId];
	ECSComponentPool::ComponentHandle handle = pool->CreateComponent(component);
	pool->GetComponent(handle)->entity = entity;
	return handle;
}

void ECS::DoRemoveComponent(uint32 componentId, component_handle handle)
{
	ECSComponentPool* pool = m_components[componentId];
	
	// Update the reference to the shifted component's offset
	BaseECSComponent* shiftedComponent = pool->back();
	component_handle oldHandle = (component_handle) (pool->size() - 1);
	Entity* entity = (Entity*) shiftedComponent->entity;
	for (uint32 i = 0; i < entity->components.size(); i++)
	{
		if (entity->components[i].id == componentId &&
			entity->components[i].handle == oldHandle)
		{
			entity->components[i].handle = handle;
			return;
		}
	}

	pool->DeleteComponent(handle);
}

uint32 ECS::FindLeastCommonComponent(const Array<uint32>& componentTypes, const Array<uint32>& componentFlags)
{
	uint32 minSize = (uint32) -1;
	uint32 minIndex = (uint32) -1;
	for (uint32 i = 0; i < componentTypes.size(); i++)
	{
		if ((componentFlags[i] & BaseECSSystem::FLAG_OPTIONAL) != 0)
			continue;

		uint32 size = m_components[componentTypes[i]]->size();
		if (size <= minSize)
		{
			minSize = size;
			minIndex = i;
		}
	}
	return minIndex;
}
