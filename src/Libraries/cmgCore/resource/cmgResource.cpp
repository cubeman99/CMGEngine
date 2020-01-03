#include "cmgResource.h"
#include <cmgCore/log/cmgLogging.h>

namespace cmg
{

Resource::Resource()
{
}

Resource::~Resource()
{
	CMG_LOG_INFO() << "Deleting resource: " << m_resourceName;
}

Error Resource::Load()
{
	CMG_LOG_INFO() << "Loading resource: " << m_resourceName;
	m_loadedState = ResourceState::k_loading;
	Error error;

	error = PreLoadImpl();
	if (error.Failed())
	{
		m_loadedState = ResourceState::k_unloaded;
		return error.Uncheck();
	}

	error = LoadImpl();
	if (error.Failed())
	{
		m_loadedState = ResourceState::k_unloaded;
		return error.Uncheck();
	}

	error = PostLoadImpl();
	if (error.Failed())
	{
		m_loadedState = ResourceState::k_unloaded;
		return error.Uncheck();
	}

	m_loadedState = ResourceState::k_loaded;
	return error.Uncheck();
}

Error Resource::Unload()
{
	CMG_LOG_INFO() << "Unloading resource: " << m_resourceName;
	m_loadedState = ResourceState::k_unloading;
	Error error;

	error = PostLoadImpl();
	if (error.Failed())
	{
		m_loadedState = ResourceState::k_loaded;
		return error.Uncheck();
	}

	error = UnloadImpl();
	if (error.Failed())
	{
		m_loadedState = ResourceState::k_loaded;
		return error.Uncheck();
	}

	error = PostUnloadImpl();
	if (error.Failed())
	{
		m_loadedState = ResourceState::k_loaded;
		return error.Uncheck();
	}

	m_loadedState = ResourceState::k_unloaded;
	return error.Uncheck();
}


}
