#ifndef _CMG_CORE_EVENT_SYSTEM_H_
#define _CMG_CORE_EVENT_SYSTEM_H_

#include <iostream>
#include <ctime>
#include <list>
#include <typeindex>
#include <cmgCore/containers/cmgArray.h>
#include <cmgCore/string/cmgString.h>

// https://medium.com/@savas/nomad-game-engine-part-7-the-event-system-45a809ccb68f

namespace cmg {

class EventSubscriber;


struct Event
{
};


// This is the interface for MemberFunctionHandler that each specialization will use
class HandlerFunctionBase
{
public:
	HandlerFunctionBase(EventSubscriber* subscriber);

	virtual void* GetInstance() = 0;
	EventSubscriber* GetSubscriber();

	void Exec(Event* evnt);

private:
	// Implemented by MemberFunctionHandler
	virtual void Call(Event * evnt) = 0;

	EventSubscriber* m_subscriber;
};


template<class T, class EventType>
class MemberFunctionHandler : public HandlerFunctionBase
{
public:
	typedef void (T::*MemberFunction)(EventType*);

	MemberFunctionHandler(EventSubscriber* subscriber,
		T* instance, MemberFunction memberFunction) :
		HandlerFunctionBase(subscriber),
		m_instance{instance},
		m_memberFunction{memberFunction}
	{
	}

	void* GetInstance() override
	{
		return m_instance;
	}

	void Call(Event* evnt) override
	{
		// Cast event to the correct type and call member function
		(m_instance->*m_memberFunction)(static_cast<EventType*>(evnt));
	}

private:
	// Pointer to class instance
	T* m_instance;

	// Pointer to member function
	MemberFunction m_memberFunction;
};


class EventManager
{
public:
	using HandlerList = std::list<HandlerFunctionBase*>;

	EventManager();
	virtual ~EventManager();

	template<typename T_Event>
	void Publish(T_Event* evnt)
	{
		HandlerList* handlersPtr = m_subscribers[typeid(T_Event)];
		if (handlersPtr == nullptr)
			return;

		// Iterate a copy of the array (in case new subscribes happen)
		HandlerList handlers = *handlersPtr;
		for (auto& handler : handlers)
		{
			if (handler != nullptr)
				handler->Exec(evnt);
		}
	}

	template<class T_Event, class T_Instance>
	void Subscribe(T_Instance* instance, void (T_Instance::*memberFunction)(T_Event*))
	{
		HandlerList* handlers = m_subscribers[typeid(T_Event)];

		// First time initialization
		if (handlers == nullptr)
		{
			handlers = new HandlerList();
			m_subscribers[typeid(T_Event)] = handlers;
		}

		instance->m_eventManagers.insert(this);
		handlers->push_back(new MemberFunctionHandler<T_Instance, T_Event>(
			instance, static_cast<T_Instance*>(instance), memberFunction));
	}

	// Unsubscribe an instance from a single event type.
	template<class T_Event, class T_Instance>
	void Unsubscribe(T_Instance* instance)
	{
		HandlerList* handlers = m_subscribers[typeid(T_Event)];
		if (handlers != nullptr)
		{
			handlers->remove_if([&](HandlerFunctionBase* x) {
				return ((T_Instance*) x->GetInstance() == instance);
			});
		}
	}

	// Unsubscribe an instance from all event types.
	template<class T_Instance>
	void UnsubscribeAll(T_Instance* instance)
	{
		for (auto it : m_subscribers)
		{
			it.second->remove_if([&](HandlerFunctionBase* x) {
				return ((T_Instance*) x->GetInstance() == instance);
			});
		}
	}

private:
	Map<std::type_index, HandlerList*> m_subscribers;
};


// Class that keeps track of event subscriptions and can auto-unsubscribe
// itself in the destructor
class EventSubscriber
{
public:
	friend class EventManager;
	friend class HandlerFunctionBase;

	virtual ~EventSubscriber();

	void UnsubscribeFromAllEvents();

private:
	Set<EventManager*> m_eventManagers;
	//Map<EventManager*, std::list<std::type_index>> m_subscriptions;
};


};

#endif // _CMG_CORE_EVENT_SYSTEM_H_