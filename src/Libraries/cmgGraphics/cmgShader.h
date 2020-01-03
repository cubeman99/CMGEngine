#ifndef _CMG_SHADER_H_
#define _CMG_SHADER_H_

#include <cmgCore/cmg_core.h>
#include <cmgCore/resource/cmgResourceLoader.h>
#include <cmgGraphics/types/cmgColor.h>

class Texture;
class Sampler;


//-----------------------------------------------------------------------------
// UniformType - Data types for shader parameters.
//-----------------------------------------------------------------------------
struct UniformType
{
	typedef int value_type;

	enum
	{
		k_unknown = -1,
		k_texture = 0,
		k_integer,
		k_unsigned_int,
		k_vec2,
		k_vec3,
		k_vec4,
		k_uvec2,
		k_uvec3,
		k_uvec4,
		k_ivec2,
		k_ivec3,
		k_ivec4,
		k_float,
		k_boolean,
		k_matrix,
		k_string,
		k_count,
	};
};



//-----------------------------------------------------------------------------
// ShaderType - Types of supported shaders stages.
//-----------------------------------------------------------------------------
enum class ShaderType
{
	k_vertex_shader = 0,
	k_fragment_shader,
	k_geometry_shader,
	k_compute_shader,
	k_count,
};


//-----------------------------------------------------------------------------
// Uniform
//-----------------------------------------------------------------------------
class Uniform
{
public:
	friend class Shader;
	typedef UniformType::value_type uniform_type;

public:
	Uniform() :
		m_name(""),
		m_type(UniformType::k_unknown),
		m_location(-1),
		m_samplerSlot(0)
	{
	}

	// Getters
	inline const String&	GetName()			const { return m_name; }
	inline uniform_type		GetType()			const { return m_type; }
	inline int				GetLocation()		const { return m_location; }
	inline int				GetSamplerSlot()	const { return m_samplerSlot; }

private:
	String m_name;
	uniform_type m_type;
	int m_location;
	int m_samplerSlot;
};


//-----------------------------------------------------------------------------
// ShaderStage
//-----------------------------------------------------------------------------
class ShaderStage
{
public:
	friend class Shader;

public:
	inline const String& GetCode() const { return m_code; }
	inline const Path& GetPath() const { return m_path; }
	inline ShaderType GetType() const { return m_type; }
	inline bool IsCompile() const { return m_isCompiled; }
	inline bool IsEnabled() const { return m_isEnabled; }
	inline uint32 GetGLShader() const { return m_glShaderName; }

	void Unload();

private:
	ShaderStage();
	~ShaderStage();
	Error Create(ShaderType type, const String& code, const Path& path);

	Path m_path;
	String m_code;
	uint32 m_glShaderName;
	bool m_isEnabled;
	bool m_isCompiled;
	ShaderType m_type;
};

struct ShaderLoadArgs
{
	Map<ShaderType, String> stages;
};

//-----------------------------------------------------------------------------
// Shader
//-----------------------------------------------------------------------------
class Shader : public cmg::ResourceArgsImpl<Texture, const ShaderLoadArgs&>
{
public:
	friend class OpenGLRenderDevice;
	using sptr = cmg::shared_ptr<Shader>;

public:
	Shader(OpenGLRenderDevice* device = nullptr);
	~Shader();
	
	// Accessors
	//const char* GetName() const;
	bool IsLinked() const;
	uint32 GetNumUniforms() const;
	bool HasUniform(const String& name) const;
	const Uniform& GetUniform(uint32 index) const;
	const Uniform* GetUniform(const String& name) const;
	int GetUniformLocation(const String& name) const;
	bool GetUniformLocation(const String& name, int& outUniformLocation) const;
	
	Error AddStage(ShaderType type, const String& code, const Path& path = "");
	Error CompileAndLink();
	Error CompileAndLink(const Array<Path>& paths);
	
	inline uint32 GetGLProgram() const { return m_glProgram; }

	static Error LoadShader(Shader*& outShader, const Path& vertexPath, const Path& fragmentPath);
	static Error LoadComputeShader(Shader*& outShader, const Path& path);
	static Error LoadComputeShader(Shader*& outShader, const Path& path, const Array<Path>& paths);

protected:
	virtual Error UnloadImpl() override;
	virtual Error LoadImpl() override;
	virtual Error LoadImpl(const ShaderLoadArgs& args) override;

private:
	template<typename T>
	Error SetUniform(const String& name, T value)
	{
		return m_device->SetShaderUniform(this, name, value);
	}
	void CreateGLProgram();
	void DeleteGLProgram();
	Error Compile(const Array<Path>& paths);
	Error PreprocessStage(ShaderStage& stage, const Array<Path>& paths);
	Error CompileStage(ShaderStage& stage);
	Error Link();
	Error Validate();
	void GenerateUniforms();
	
public:

	OpenGLRenderDevice* m_device;
	bool m_isLinked;
	uint32 m_glProgram;
	ShaderStage m_stages[(int) ShaderType::k_count];
	Array<Uniform> m_uniforms;
};

#endif // _CMG_SHADER_H_