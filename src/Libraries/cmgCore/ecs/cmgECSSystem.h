#ifndef _CMG_CORE_ECS_SYSTEM_H_
#define _CMG_CORE_ECS_SYSTEM_H_

#include <cmgCore/containers/cmgArray.h>
#include <cmgCore/ecs/cmgECSComponent.h>


class BaseECSSystem
{
public:
	enum Flags
	{
		FLAG_NONE = 0,
		FLAG_OPTIONAL = 1,
	};

public:
	BaseECSSystem()
	{
	}

	bool IsValid();

	inline const Array<uint32>& GetComponentTypes()
	{
		return m_componentTypes;
	}

	inline const Array<uint32>& GetComponentFlags()
	{
		return m_componentFlags;
	}

	virtual void UpdateComponents(float timeDelta,
		BaseECSComponent** components)
	{
	}

	virtual void PreUpdate(float timeDelta)
	{
	}

	virtual void PostUpdate(float timeDelta)
	{
	}

protected:
	void AddComponentType(uint32 componentType, uint32 componentFlag = FLAG_NONE)
	{
		m_componentTypes.push_back(componentType);
		m_componentFlags.push_back(componentFlag);
	}

	template <class T_Component>
	void AddComponentType(uint32 componentFlag = FLAG_NONE)
	{
		m_componentTypes.push_back(T_Component::ID);
		m_componentFlags.push_back(componentFlag);
	}

private:
	Array<uint32> m_componentTypes;
	Array<uint32> m_componentFlags;
};

class ECSSystemList
{
public:
	inline bool AddSystem(BaseECSSystem& system)
	{
		if (!system.IsValid())
		{
			return false;
		}
		m_systems.push_back(&system);
		return true;
	}

	inline size_t Size()
	{
		return m_systems.size();
	}

	inline BaseECSSystem* operator[](uint32 index)
	{
		return m_systems[index];
	}

	bool RemoveSystem(BaseECSSystem& system);

private:
	Array<BaseECSSystem*> m_systems;
};


#endif // _CMG_CORE_ECS_SYSTEM_H_