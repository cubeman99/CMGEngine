#include "cmgResourceManager.h"

namespace cmg {

	ResourceManager::ResourceManager():
		m_resourceIdCounter(1)
	{
	}

	ResourceManager::~ResourceManager()
	{
	}

	Error ResourceManager::LoadTexture(resource_ptr<Texture>& outTexture,
		const Path& path, const TextureParams& params)
	{
		Texture* texture = nullptr;
		ResourceName name = path.ToString();
		Error error = Texture::LoadTexture(texture, ResolvePath(path), params);
		if (error.Passed())
			outTexture = Add(name, texture);
		return error;
	}

	Error ResourceManager::LoadMesh(resource_ptr<Mesh>& outMesh, const Path& path,
		MeshLoadOptions::value_type options)
	{
		Mesh* mesh = nullptr;
		ResourceName name = path.ToString();
		Error error = Mesh::Load(ResolvePath(path), mesh, options);
		if (error.Passed())
			outMesh = Add(name, mesh);
		return error;
	}

	Error ResourceManager::LoadShader(resource_ptr<Shader>& outShader,
		const ResourceName& name, const Path& vertexPath, const Path& fragmentPath)
	{
		Shader* shader = new Shader();
		Error error = Shader::LoadShader(shader,
			ResolvePath(vertexPath),
			ResolvePath(fragmentPath));
		if (error.Passed())
			outShader = Add(name, shader);
		return error;
	}

	Error ResourceManager::LoadComputeShader(resource_ptr<Shader>& outShader,
		const Path& path)
	{
		Shader* shader = new Shader();
		ResourceName name = path.ToString();
		Error error = Shader::LoadComputeShader(shader,
			ResolvePath(path), m_shaderIncludePaths);
		if (error.Passed())
			outShader = Add(name, shader);
		return error;
	}

	Error ResourceManager::LoadBuiltInFont(resource_ptr<SpriteFont>& outSpriteFont, BuiltInFonts builtInFont)
	{
		SpriteFont* font = SpriteFont::LoadBuiltInFont(builtInFont);
		outSpriteFont = Add(font);
		return CMG_ERROR_SUCCESS;
	}

	Error ResourceManager::LoadFont(resource_ptr<Font>& outFont,
		const Path& path, uint32 size, uint32 charRegionBegin, uint32 charRegionEnd)
	{
		Font* font = nullptr;
		ResourceName name = path.ToString();
		Error error = Font::LoadFont(font, ResolvePath(path),
			size, charRegionBegin, charRegionEnd);
		if (error.Passed())
			outFont = Add(name, font);
		return error.Uncheck();
	}

	void ResourceManager::AddShaderIncludePath(const Path& path)
	{
		m_shaderIncludePaths.push_back(path);
	}

	void ResourceManager::AddPath(const Path & path)
	{
		if (std::find(m_paths.begin(), m_paths.end(), path) == m_paths.end())
			m_paths.push_back(path);
	}

	Path ResourceManager::ResolvePath(const Path & path)
	{
		return Path::ResolvePath(path, m_paths);
	}

	template<> ResourcePool<Texture>* ResourceManager::GetResourcePool<Texture>()
	{
		return &m_poolTextures;
	}

	template<> ResourcePool<Mesh>* ResourceManager::GetResourcePool<Mesh>()
	{
		return &m_poolMeshes;
	}

	template<> ResourcePool<Shader>* ResourceManager::GetResourcePool<Shader>()
	{
		return &m_poolShaders;
	}

	template<> ResourcePool<SpriteFont>* ResourceManager::GetResourcePool<SpriteFont>()
	{
		return &m_poolSpriteFonts;
	}

	template<> ResourcePool<Font>* ResourceManager::GetResourcePool<Font>()
	{
		return &m_poolFonts;
	}
}
