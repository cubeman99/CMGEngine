#include "cmgECS.h"
#include "cmgAssert.h"
#include <cmgMath/cmgMathLib.h>


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

