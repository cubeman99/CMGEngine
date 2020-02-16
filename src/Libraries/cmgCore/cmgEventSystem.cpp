#include "cmgEventSystem.h"

namespace cmg {

HandlerFunctionBase::HandlerFunctionBase(EventSubscriber* subscriber) :
	m_subscriber(subscriber)
{
}

void HandlerFunctionBase::Exec(Event * evnt)
{
	Call(evnt);
}

EventSubscriber* HandlerFunctionBase::GetSubscriber()
{
	return m_subscriber;
}

EventManager::EventManager()
{
}

EventManager::~EventManager()
{
	// Remove self from all subscribers, so they don't try to unsubscribe
	// themselves from this deleted object.
	Set<EventSubscriber*> subscribers;
	for (auto it : m_subscribers)
	{
		if (it.second)
		{
			HandlerList& handlers = *it.second;
			for (auto handler : handlers)
			{
				if (handler->GetSubscriber())
					subscribers.insert(handler->GetSubscriber());
			}
		}
	}
	for (auto subscriber : subscribers)
		subscriber->m_eventManagers.erase(this);
}

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