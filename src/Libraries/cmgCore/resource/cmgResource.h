#ifndef _CMG_CORE_RESOURCE_H_
#define _CMG_CORE_RESOURCE_H_

#include <cmgCore/error/cmgError.h>
#include <cmgCore/smart_ptr/cmgSharedPtr.h>
#include <cmgCore/io/cmgFile.h>
#include <cmgCore/log/cmgLogging.h>

namespace cmg
{

class Resource;
using ResourceHandle = int32;
using ResourcePtr = Resource*;


template <typename T>
using resource_ptr = cmg::shared_ptr<T>;

enum class ResourceState
{
	k_unloaded,
	k_unloading,
	k_loading,
	k_loaded,
};


class Resource
{
public:
	template <class T>
	friend class ResourceLoader;

	using sptr = resource_ptr<Resource>;

public:
	Resource();
	virtual ~Resource();

	ResourceHandle GetResourceHandle() const { return m_resourceHandle; }
	const String& GetResourceName() const { return m_resourceName; }
	ResourceState GetLoadedState() const { return m_loadedState; }
	bool IsLoaded() const { return m_loadedState == ResourceState::k_loaded; }

	virtual Error Load();
	Error Unload();

protected:
	virtual Error PreLoadImpl() { return CMG_ERROR_SUCCESS; }
	virtual Error LoadImpl() { return CMG_ERROR_SUCCESS; }
	virtual Error PostLoadImpl() { return CMG_ERROR_SUCCESS; }
	virtual Error UnloadImpl() { return CMG_ERROR_SUCCESS; }
	virtual Error PreUnloadImpl() { return CMG_ERROR_SUCCESS; }
	virtual Error PostUnloadImpl() { return CMG_ERROR_SUCCESS; }

protected:
	void* m_resourceLoader = nullptr;
	String m_resourceName = "";
	ResourceHandle m_resourceHandle = -1;
	ResourceState m_loadedState = ResourceState::k_unloaded;
};


template <class T>
class ResourceImpl : public Resource
{
public:
	using sptr = resource_ptr<T>;

	ResourceLoader<T>* GetResourceLoader()
	{
		return (ResourceLoader<T>*) m_resourceLoader;
	}
};


template <class T, typename T_LoadArgs>
class ResourceArgsImpl : public ResourceImpl<T>
{
public:
	// Necessary to define this for Load to be overridden with templates
	Error Load() override
	{
		return Resource::Load();
	}

	virtual Error LoadImpl(T_LoadArgs args) = 0;

	template <typename T_LoadArgs>
	Error Load(T_LoadArgs args)
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

		error = LoadImpl(args);
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
};

}

#endif // _CMG_CORE_RESOURCE_H_