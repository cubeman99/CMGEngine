#include "cmgEventSystem.h"

namespace cmg {


EventSubscriber::~EventSubscriber()
{
	UnsubscribeFromAllEvents();
}

void EventSubscriber::UnsubscribeFromAllEvents()
{
	for (EventManager* bus : m_eventManagers)
		bus->UnsubscribeAll(this);
	m_eventManagers.clear();
}


}