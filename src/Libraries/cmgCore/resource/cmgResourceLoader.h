#ifndef _CMG_CORE_RESOURCE_LOADER_H_
#define _CMG_CORE_RESOURCE_LOADER_H_

#include <cmgCore/log/cmgLogging.h>
#include <cmgCore/resource/cmgResource.h>
#include <typeindex>

namespace cmg
{
class ResourceManagerBase;

class ResourceLoaderBase
{
public:
	ResourceLoaderBase(ResourceManagerBase* resourceManager);
	virtual ~ResourceLoaderBase();

	ResourceManagerBase* GetResourceManager() const { return m_resourceManager; }

	Path GetResourcePath(const String& name);
	Error OpenResourceFile(File& outFile, const String& name,
		FileAccess access = FileAccess::READ,
		FileType type = FileType::BINARY);

	void AddPath(const Path& path);
	Path ResolvePath(const Path& path);

private:
	ResourceManagerBase* m_resourceManager;
	Array<Path> m_paths;
	Array<Path> m_shaderIncludePaths;
};

template <class T>
class ResourceLoader : public ResourceLoaderBase
{
public:
	ResourceLoader(ResourceManagerBase* resourceManager) :
		ResourceLoaderBase(resourceManager)
	{
	}

	virtual ~ResourceLoader()
	{
		UnloadAll();
	}

	void UnloadAll()
	{
		for (auto resource : m_resources)
		{
			Unload(resource);
		}
	}

	virtual T* CreateImpl(const String& name)
	{
		return new T();
	}

	resource_ptr<T> Get(const String& name)
	{
		auto it = m_resourceMap.find(name);
		if (it != m_resourceMap.end())
			return it->second;
		return Create(name);
	}

	bool Contains(const String& name) const
	{
		auto it = m_resourceMap.find(name);
		return (it != m_resourceMap.end());
	}

	resource_ptr<T> Load(const String& name)
	{
		resource_ptr<T> resource = Get(name);
		if (!resource->IsLoaded())
			resource->Load();
		return resource;
	}

	template <typename T_LoadArgs>
	resource_ptr<T> Load(const String& name, T_LoadArgs args)
	{
		resource_ptr<T> resource = Get(name);
		if (!resource->IsLoaded())
			resource->Load(args);
		return resource;
	}

	resource_ptr<T> Create(const String& name, T* resourcePtr)
	{
		ResourceHandle handle = static_cast<ResourceHandle>(m_resources.size());

		// Check if there exists a resource with this name
		auto it = m_resourceMap.find(name);
		if (it != m_resourceMap.end())
		{
			it->second->Unload();
			handle = it->second->m_resourceHandle;
		}

		resource_ptr<T> resource = resource_ptr<T>(resourcePtr);
		resource->m_resourceHandle = handle;
		resource->m_resourceName = name;
		resource->m_resourceLoader = this;

		m_resourceMap[name] = resource;
		m_resources.push_back(resource);
		return resource;
	}

	void Unload(const String& name)
	{
		resource_ptr<T> resource = Get(name);
		Unload(resource);
	}

	resource_ptr<T> Reload(const String& name)
	{
		resource_ptr<T> resource = Get(name);
		resource->Unload();
		resource->Load();
		return resource;
	}

private:
	void Unload(ResourceHandle handle)
	{
		resource_ptr<T> resource = Get(handle);
		Unload(resource);
	}
	void Unload(resource_ptr<T> resource)
	{
		if (resource->IsLoaded())
		{
			resource->Unload();
		}
	}

	resource_ptr<T> Get(ResourceHandle handle)
	{
		return m_resources[handle];
	}

	resource_ptr<T> Create(const String& name)
	{
		CMG_LOG_INFO() << "Creating resource: " << name;
		ResourceHandle handle = static_cast<ResourceHandle>(m_resources.size());
		return Create(name, CreateImpl(name));
	}

	Map<String, resource_ptr<T>> m_resourceMap;
	Array<resource_ptr<T>> m_resources;
};



class ResourceManagerBase
{
public:
	ResourceManagerBase();
	virtual ~ResourceManagerBase();

	template <class T>
	void RegisterResourceType();

	// Resource access
	template <class T>
	resource_ptr<T> Load(const String& name);
	template <class T_Resource, typename T_LoadArgs>
	resource_ptr<T_Resource> Load(const String& name, T_LoadArgs args);
	template <class T>
	resource_ptr<T> Create(const String& name);
	template <class T>
	resource_ptr<T> Get(const String& name);
	template <class T>
	resource_ptr<T> Add(const String& name, T* resource);
	template <class T>
	ResourceLoader<T>* GetResourcePool();

protected:
	Map<std::type_index, ResourceLoaderBase*> m_resourceLoaders;
};

template<class T>
inline void ResourceManagerBase::RegisterResourceType()
{
	ResourceLoader<T>* loader = new ResourceLoader<T>(this);
	m_resourceLoaders[typeid(T)] = loader;
}

template <class T>
resource_ptr<T> ResourceManagerBase::Get(const String& path)
{
	return GetResourcePool<T>()->Get(path);
}

template <class T>
resource_ptr<T> ResourceManagerBase::Add(const String& name, T* resource)
{
	return GetResourcePool<T>()->Create(name, resource);
}

template<class T>
inline resource_ptr<T> ResourceManagerBase::Load(const String& name)
{
	return GetResourcePool<T>()->Load(name);
}

template<class T_Resource, typename T_LoadArgs>
inline resource_ptr<T_Resource> ResourceManagerBase::Load(const String& name, T_LoadArgs args)
{
	return GetResourcePool<T_Resource>()->Load<T_LoadArgs>(name, args);
}

template <class T>
resource_ptr<T> ResourceManagerBase::Create(const String& name)
{
	return GetResourcePool<T>()->Get(name);
}

template <class T>
ResourceLoader<T>* ResourceManagerBase::GetResourcePool()
{
	return static_cast<ResourceLoader<T>*>(m_resourceLoaders[typeid(T)]);
}

}

#endif // _CMG_CORE_RESOURCE_LOADER_H_