#include "cmgResourceLoader.h"

namespace cmg
{

ResourceLoaderBase::ResourceLoaderBase(ResourceManagerBase* resourceManager) :
	m_resourceManager(resourceManager)
{
}

ResourceLoaderBase::~ResourceLoaderBase()
{
}

Path ResourceLoaderBase::GetResourcePath(const String& name)
{
	return ResolvePath(name);
}

Error ResourceLoaderBase::OpenResourceFile(File& outFile,
	const String& name, FileAccess access, FileType type)
{
	Path path = ResolvePath(name);
	outFile.Close();
	return outFile.Open(path, access, type);
}

void ResourceLoaderBase::AddPath(const Path & path)
{
	if (std::find(m_paths.begin(), m_paths.end(), path) == m_paths.end())
		m_paths.push_back(path);
}

Path ResourceLoaderBase::ResolvePath(const Path & path)
{
	return Path::ResolvePath(path, m_paths);
}

ResourceManagerBase::ResourceManagerBase()
{
}

ResourceManagerBase::~ResourceManagerBase()
{
	for (auto it : m_resourceLoaders)
		delete it.second;
}

}

