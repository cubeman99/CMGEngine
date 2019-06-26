#ifndef _CMG_SHADER_H_
#define _CMG_SHADER_H_

#include <cmgGraphics/types/cmgColor.h>
#include <cmgCore/cmg_core.h>

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
		k_unsigned_integer,
		k_vec2,
		k_vec3,
		k_vec4,
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
	//typedef int value_type;

	//enum
	//{
		k_vertex_shader = 0,
		k_fragment_shader,
		k_geometry_shader,
		k_compute_shader,
		k_count,
	//};
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
	String			m_name;
	uniform_type	m_type;
	int				m_location;
	int				m_samplerSlot;
};


//-----------------------------------------------------------------------------
// Shader
//-----------------------------------------------------------------------------
class Shader
{
public:
	friend class OpenGLRenderDevice;
	//typedef ShaderType::value_type shader_type;

public:
	Shader(OpenGLRenderDevice* device = nullptr);
	~Shader();
	
	// Accessors.
	//const char*		GetName() const;
	bool				IsLinked() const;
	unsigned int		GetNumUniforms() const;
	const Uniform&		GetUniform(unsigned int index) const;
	const Uniform*		GetUniform(const String& name) const;
	int					GetUniformLocation(const String& name) const;
	bool				GetUniformLocation(const String& name, int& outUniformLocation) const;
			
	Error AddStage(ShaderType type, const String& code, const String& fileName = "");
	Error CompileAndLink();
	
	template<typename T>
	Error SetUniform(const String& name, T value)
	{
		return m_device->SetShaderUniform(this, name, value);
	}

	Error SetSampler(const String& samplerName,
		Texture* texture, Sampler* sampler, uint32 slot);
	//Error SetUniformBuffer(const String& name, UniformBuffer* buffer);

	inline unsigned int GetGLProgram() const { return m_glProgram; }

	static Error LoadShader(Shader*& outShader, const Path& vertexPath, const Path& fragmentPath);
	static Error LoadComputeShader(Shader*& outShader, const Path& path);

private:
	Error Compile();
	Error CompileStage(ShaderType stage);
	Error Link();
	Error Validate();
	void GenerateUniforms();
	
public:
	OpenGLRenderDevice* m_device;
	String			m_shaderStageFileNames[(int) ShaderType::k_count];
	bool			m_isLinked;
	Array<Uniform>	m_uniforms;
	unsigned int	m_glProgram;
	unsigned int	m_glShaderStages[(int) ShaderType::k_count];
};


#endif // _CMG_SHADER_H_