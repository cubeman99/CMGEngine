#ifndef _CMG_APP_RESOURCE_MANAGER_H_
#define _CMG_APP_RESOURCE_MANAGER_H_

#include <cmgCore/resource/cmgResourceLoader.h>
#include <cmgGraphics/cmgTexture.h>
#include <cmgGraphics/cmgMesh.h>
#include <cmgGraphics/cmgShader.h>
#include <cmgGraphics/cmgFont.h>
#include <cmgGraphics/cmgModel.h>
#include <cmgGraphics/cmgAnimationClip.h>
#include <unordered_map>

namespace cmg
{

class ResourceManager : public ResourceManagerBase
{
public:
	ResourceManager();
	virtual ~ResourceManager();

	// Loading functions
	Error LoadShader(resource_ptr<Shader>& outShader, const String& name,
		const Path& vertexPath, const Path& fragmentPath);
	Error LoadComputeShader(resource_ptr<Shader>& outShader, const Path& path);
	Error LoadBuiltInFont(resource_ptr<Font>& outFont, BuiltInFonts builtInFont);
	Error LoadFont(resource_ptr<Font>& outFont, const Path& path,
		uint32 size, uint32 charRegionBegin, uint32 charRegionEnd);

	// Paths
	void AddShaderIncludePath(const Path& path);
	void AddPath(const Path& path);
	Path ResolvePath(const Path& path);

private:
	Array<Path> m_paths;
	Array<Path> m_shaderIncludePaths;
};


}

#endif // _CMG_APP_RESOURCE_MANAGER_H_