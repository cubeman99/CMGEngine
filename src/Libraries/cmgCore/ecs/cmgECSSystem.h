#ifndef _CMG_CORE_ECS_SYSTEM_H_
#define _CMG_CORE_ECS_SYSTEM_H_

#include <cmgCore/containers/cmgArray.h>
#include <cmgCore/ecs/cmgECSComponent.h>
#include <tuple>

class BaseECSSystem
{
public:
	enum Flags
	{
		FLAG_NONE = 0,
		FLAG_OPTIONAL = 1,
	};

public:
	BaseECSSystem(const Array<uint32>& componentTypesIn) :
		m_componentTypes(componentTypesIn)
	{
	}

	virtual void UpdateComponents(float timeDelta, BaseECSComponent** components)
	{
	}

	inline const Array<uint32>& GetComponentTypes()
	{
		return m_componentTypes;
	}

	inline const Array<uint32>& GetComponentFlags()
	{
		return m_componentFlags;
	}

private:
	Array<uint32> m_componentTypes;
	Array<uint32> m_componentFlags;
};

class ECSSystemList
{
public:
	inline bool addSystem(BaseECSSystem& system)
	{
		//if (!system.IsValid())
		//{
		//	return false;
		//}
		systems.push_back(&system);
		return true;
	}
	inline size_t size()
	{
		return systems.size();
	}
	inline BaseECSSystem* operator[](uint32 index)
	{
		return systems[index];
	}
	bool removeSystem(BaseECSSystem& system);
private:
	Array<BaseECSSystem*> systems;
};


#endif // _CMG_CORE_ECS_SYSTEM_H_