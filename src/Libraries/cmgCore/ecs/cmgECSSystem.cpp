#include "cmgECSSystem.h"


bool BaseECSSystem::IsValid()
{
	for (uint32 i = 0; i < m_componentFlags.size(); i++)
	{
		if ((m_componentFlags[i] & BaseECSSystem::FLAG_OPTIONAL) == 0)
		{
			return true;
		}
	}
	return false;
}

bool ECSSystemList::RemoveSystem(BaseECSSystem& system)
{
	for (uint32 i = 0; i < m_systems.size(); i++)
	{
		if (m_systems[i] == &system)
		{
			m_systems.erase(m_systems.begin() + i);
			return true;
		}
	}
	return false;
}
