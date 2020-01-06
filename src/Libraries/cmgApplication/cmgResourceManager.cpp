#include "cmgResourceManager.h"

namespace cmg {

	ResourceManager::ResourceManager()
	{
		RegisterResourceType<Texture>();
		RegisterResourceType<Mesh>();
		RegisterResourceType<Model>();
		RegisterResourceType<Font>();
		RegisterResourceType<Shader>();
		RegisterResourceType<AnimationClip>();
	}

	ResourceManager::~ResourceManager()
	{
	}

	Texture::sptr ResourceManager::LoadCubeMapTexture(Path paths[6])
	{
		TextureParams params(TextureTarget::k_texture_cube_map);
		Texture::sptr texture = cmg::make_shared<Texture>(params);
		for (uint32 face = 0; face < 6; face++)
		{
			//paths[face];
		}
		return texture;
	}

	Error ResourceManager::LoadShader(resource_ptr<Shader>& outShader,
		const String& name, const Path& vertexPath, const Path& fragmentPath)
	{
		ShaderLoadArgs args;
		args.stages[ShaderType::k_vertex_shader] = vertexPath;
		args.stages[ShaderType::k_fragment_shader] = fragmentPath;
		outShader = Load<Shader>(name, args);
		return CMG_ERROR_SUCCESS;
	}

	Error ResourceManager::LoadComputeShader(resource_ptr<Shader>& outShader,
		const Path& path)
	{
		ShaderLoadArgs args;
		args.stages[ShaderType::k_compute_shader] = path;
		outShader = Load<Shader>(path, args);
		return CMG_ERROR_SUCCESS;
	}

	Error ResourceManager::LoadBuiltInFont(resource_ptr<Font>& outFont, BuiltInFonts builtInFont)
	{
		Font* font = nullptr;
		Error error = Font::LoadBuiltInFont(font, builtInFont);
		std::stringstream ss;
		ss << ".cmg/built_in_fonts/" << (int) builtInFont;
		String name = ss.str();
		if (error.Passed())
			outFont = Add(name, font);
		return error.Uncheck();
	}

	Error ResourceManager::LoadFont(resource_ptr<Font>& outFont,
		const Path& path, uint32 size, uint32 charRegionBegin, uint32 charRegionEnd)
	{
		Font* font = nullptr;
		String name = path.ToString();
		CMG_LOG_INFO() << "Loading font: " << path;
		Error error = Font::LoadFont(font, ResolvePath(path),
			size, charRegionBegin, charRegionEnd);
		if (error.Passed())
			outFont = Add(name, font);
		else
			delete font;
		return error.Uncheck();
	}

	void ResourceManager::AddShaderIncludePath(const Path& path)
	{
		m_shaderIncludePaths.push_back(path);
	}

	void ResourceManager::AddPath(const Path& path)
	{
		if (std::find(m_paths.begin(), m_paths.end(), path) == m_paths.end())
			m_paths.push_back(path);
		for (auto it : m_resourceLoaders)
			it.second->AddPath(path);
	}

	Path ResourceManager::ResolvePath(const Path& path)
	{
		return Path::ResolvePath(path, m_paths);
	}

}
