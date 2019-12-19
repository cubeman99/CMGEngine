#ifndef _CMG_APP_RESOURCE_MANAGER_H_
#define _CMG_APP_RESOURCE_MANAGER_H_

#include <cmgCore/smart_ptr/cmg_smart_ptr.h>
#include <cmgCore/io/cmgPath.h>
#include <cmgGraphics/cmgTexture.h>
#include <cmgGraphics/cmgMesh.h>
#include <cmgGraphics/cmgShader.h>
#include <cmgGraphics/cmgSpriteFont.h>
#include <unordered_map>

namespace cmg
{

using ResourceName = String;
using ResourceID = uint32;
constexpr ResourceID NULL_RESOURCE_ID = ResourceID(0);


template <class T>
class Resource
{
public:
	Resource():
		m_isLoaded(false),
		m_id(NULL_RESOURCE_ID)
	{
	}
	Resource(T* ptr, const ResourceName& name, ResourceID id):
		m_ptr(ptr),
		m_isLoaded(true),
		m_name(name),
		m_id(id)
	{
	}
	~Resource()
	{
		Unload();
	}
	bool IsLoaded() const
	{
		return m_isLoaded;
	}
	void Reset(T* ptr)
	{
		Unload();
		m_ptr.reset(ptr);
		m_isLoaded = true;
	}
	void Unload()
	{
		if (m_isLoaded)
		{
			m_ptr.reset();
			m_isLoaded = false;
			std::cout << "Unloaded resource " << m_name << "\n";
		}
	}

	shared_ptr<T> Get() const
	{
		return shared_ptr<T>(m_ptr);
	}

private:
	ResourceID m_id;
	ResourceName m_name;
	shared_ptr<T> m_ptr;
	bool m_isLoaded;
};

template <class T>
struct resource_free
{
	void operator()(Resource<T>* resource)
	{
		resource->Unload();
		delete resource;
	}
};


template <class T>
class ResourcePool
{
public:
	using resource_t = Resource<T>;
	using resource_ptr = shared_ptr<Resource<T>>;
	using external_ptr = shared_ptr<T>;

public:
	ResourcePool():
		m_autoNameGenCounter(1)
	{
	}

	String AutoGenerateName()
	{
		std::stringstream ss;
		ss << "auto_gen_resource_" << m_autoNameGenCounter;
		m_autoNameGenCounter++;
		return ss.str();
	}

	external_ptr Add(ResourceID id, T* resource)
	{
		ResourceName name = AutoGenerateName();
		CMG_ASSERT(resource != nullptr);
		CMG_ASSERT(m_resourceMap.find(name) == m_resourceMap.end());
		m_resourceMap[name] = std::make_shared<resource_t>(resource, name, id);
		return m_resourceMap[name]->Get();
	}

	external_ptr Add(const ResourceName& name, ResourceID id, T* resource)
	{
		CMG_ASSERT(resource != nullptr);
		CMG_ASSERT(m_resourceMap.find(name) == m_resourceMap.end());
		m_resourceMap[name] = std::make_shared<resource_t>(resource, name, id);
		return m_resourceMap[name]->Get();
	}

	external_ptr Set(const ResourceName& name, T* ptr)
	{
		CMG_ASSERT(ptr != nullptr);
		CMG_ASSERT(m_resourceMap.find(name) != m_resourceMap.end());
		resource_ptr prevResource = m_resourceMap[name];
		prevResource->Reset(ptr);
		return prevResource->Get();
	}

	bool Contains(const ResourceName& name)
	{
		return m_resourceMap.find(name) != m_resourceMap.end();
	}

	external_ptr Get(const ResourceName& name)
	{
		if (m_resourceMap.find(name) == m_resourceMap.end())
			return external_ptr();
		return m_resourceMap.at(name)->Get();
	}

	bool IsLoaded(const ResourceName& name)
	{
		if (m_resourceMap.find(name) == m_resourceMap.end())
			return false;
		return m_resourceMap.at(name)->IsLoaded();
	}

	void Unload(const ResourceName& name)
	{
		CMG_ASSERT(m_resourceMap.find(name) != m_resourceMap.end());
		m_resourceMap.at(name)->Unload();
	}

private:
	uint32 m_autoNameGenCounter;
	std::map<String, resource_ptr> m_resourceMap;
};
	

class ResourceManager
{
public:
	template <class T>
	using resource_ptr = shared_ptr<T>;
		
public:
	ResourceManager();
	virtual ~ResourceManager();

	template <class T>
	resource_ptr<T> Create(const ResourceName& name);
	template <class T>
	resource_ptr<T> Get(const ResourceName& name);
	template <class T>
	resource_ptr<T> Add(T* resource);
	template <class T>
	resource_ptr<T> Add(const ResourceName& name, T* resource);
	template <class T>
	ResourcePool<T>* GetResourcePool();

	Error LoadMesh(resource_ptr<Mesh>& mesh, const Path& path,
		MeshLoadOptions::value_type options = MeshLoadOptions::k_none);
	Error LoadTexture(resource_ptr<Texture>& outTexture,
		const Path& path, const TextureParams& params = TextureParams());
	Error LoadShader(resource_ptr<Shader>& outShader, const ResourceName& name,
		const Path& vertexPath, const Path& fragmentPath);
	Error LoadComputeShader(resource_ptr<Shader>& outShader, const Path& path);
	Error LoadBuiltInFont(resource_ptr<SpriteFont>& outSpriteFont, BuiltInFonts builtInFont);

	void AddShaderIncludePath(const Path& path);
	void AddPath(const Path& path);
	Path ResolvePath(const Path& path);

private:
	ResourceID GetNewID()
	{
		ResourceID id = m_resourceIdCounter;
		m_resourceIdCounter++;
		return id;
	}

	ResourceID m_resourceIdCounter;
	ResourcePool<Texture> m_poolTextures;
	ResourcePool<Mesh> m_poolMeshes;
	ResourcePool<Shader> m_poolShaders;
	ResourcePool<SpriteFont> m_poolFonts;
	Array<Path> m_paths;
	Array<Path> m_shaderIncludePaths;
};

template <class T>
ResourceManager::resource_ptr<T> ResourceManager::Get(const ResourceName& path)
{
	ResourcePool<T>* pool = GetResourcePool<T>();
	return pool->Get(path);
}

template <class T>
ResourceManager::resource_ptr<T> ResourceManager::Add(T* resource)
{
	ResourcePool<T>* pool = GetResourcePool<T>();
	return pool->Add(GetNewID(), resource);
}

template <class T>
ResourceManager::resource_ptr<T> ResourceManager::Add(const ResourceName& name, T* resource)
{
	ResourcePool<T>* pool = GetResourcePool<T>();
	if (pool->Contains(name))
		return pool->Set(name, resource);
	else
		return pool->Add(name, GetNewID(), resource);
}

template <class T>
ResourceManager::resource_ptr<T> ResourceManager::Create(const ResourceName& name)
{
	ResourcePool<T>* pool = GetResourcePool<T>();
	CMG_ASSERT(!pool->Contains(name))
	T* resource = new T();
	return pool->Add(name, GetNewID(), resource);
}

template <class T>
ResourcePool<T>* ResourceManager::GetResourcePool()
{
	return nullptr;
}

template<> ResourcePool<Texture>* ResourceManager::GetResourcePool<Texture>();
template<> ResourcePool<Mesh>* ResourceManager::GetResourcePool<Mesh>();
template<> ResourcePool<Shader>* ResourceManager::GetResourcePool<Shader>();
template<> ResourcePool<SpriteFont>* ResourceManager::GetResourcePool<SpriteFont>();

}

#endif // _CMG_APP_RESOURCE_MANAGER_H_