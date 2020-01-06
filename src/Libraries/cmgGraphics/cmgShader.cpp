#include "cmgShader.h"
#include <cmgGraphics/cmgOpenGLIncludes.h>
#include <cmgGraphics/render/cmgSampler.h>
#include <cmgGraphics/cmgTexture.h>
#include <cmgGraphics/render/cmgRenderDevice.h>
#include <assert.h>
#include <regex>

static const char* GetShaderStageName(ShaderType type) 
{
	if (type == ShaderType::k_vertex_shader)
		return "vertex";
	else if (type == ShaderType::k_fragment_shader)
		return "fragment";
	else if (type == ShaderType::k_geometry_shader)
		return "geometry";
	else if (type == ShaderType::k_compute_shader)
		return "compute";
	CMG_ASSERT_FALSE("invalid shader stage type");
	return "unknown";
}

ShaderStage::ShaderStage():
	m_isEnabled(false),
	m_isCompiled(false),
	m_glShaderName(0)
{
}

ShaderStage::~ShaderStage()
{
	Unload();
}

void ShaderStage::Unload()
{
	if (m_glShaderName != 0)
	{
		glDeleteShader(m_glShaderName);
		m_glShaderName = 0;
	}
}

Error ShaderStage::Create(ShaderType type, const String& code, const Path& path)
{
	m_isEnabled = true;
	m_type = type;
	m_path = path;
	m_code = code;

	// Create the correct shader object.
	if (type == ShaderType::k_vertex_shader)
		m_glShaderName = glCreateShader(GL_VERTEX_SHADER);
	else if (type == ShaderType::k_fragment_shader)
		m_glShaderName = glCreateShader(GL_FRAGMENT_SHADER);
	else if (type == ShaderType::k_geometry_shader)
		m_glShaderName = glCreateShader(GL_GEOMETRY_SHADER);
	else if (type == ShaderType::k_compute_shader)
		m_glShaderName = glCreateShader(GL_COMPUTE_SHADER);
	else
	{
		CMG_ASSERT_FALSE("Invalid shader stage type.");
		return CMG_ERROR_FAILURE;
	}

	return CMG_ERROR_SUCCESS;
}


//-----------------------------------------------------------------------------
// Constructor & destructor
//-----------------------------------------------------------------------------

Shader::Shader(OpenGLRenderDevice* device):
	m_isLinked(false),
	m_device(device),
	m_glProgram(0)
{
}

Shader::~Shader()
{
	DeleteGLProgram();
}



//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------

bool Shader::IsLinked() const
{
	return m_isLinked;
}

uint32 Shader::GetNumUniforms() const
{
	return m_uniforms.size();
}

bool Shader::HasUniform(const String & name) const
{
	return GetUniform(name) != nullptr;
}

const Uniform& Shader::GetUniform(uint32 index) const
{
	return m_uniforms[index];
}

const Uniform* Shader::GetUniform(const String& name) const
{
	for (uint32 i = 0; i < m_uniforms.size(); ++i)
	{
		if (m_uniforms[i].m_name == name)
			return &m_uniforms[i];
	}
	return nullptr;
}

int Shader::GetUniformLocation(const String& name) const
{
	for (uint32 i = 0; i < m_uniforms.size(); ++i)
	{
		if (m_uniforms[i].m_name == name)
			return m_uniforms[i].GetLocation();
	}
	return -1;
}

bool Shader::GetUniformLocation(const String& name, int& outUniformLocation) const
{
	for (uint32 i = 0; i < m_uniforms.size(); ++i)
	{
		if (m_uniforms[i].m_name == name)
		{
			outUniformLocation = m_uniforms[i].GetLocation();
			return true;
		}
	}
	return false;
}


//-----------------------------------------------------------------------------
// Compiling and Linking
//-----------------------------------------------------------------------------

Error Shader::AddStage(ShaderType type, const String& code, const Path& path)
{
	// Create the stage
	ShaderStage& stage = m_stages[(int) type];
	Error error = stage.Create(type, code, path);
	if (error.Failed())
		return error.Uncheck();

	CreateGLProgram();

	// Attach the shader stage to the shader program
	glAttachShader(m_glProgram, stage.m_glShaderName);
	m_isLinked = false;

	return CMG_ERROR_SUCCESS;
}

Error Shader::CompileAndLink()
{
	Array<Path> paths;
	return CompileAndLink(paths);
}

Error Shader::CompileAndLink(const Array<Path>& paths)
{
	Error error;

	// 1. Compile each stage
	error = Compile(paths);
	if (error.Failed())
	{
		error.Uncheck();
		return error;
	}

	// 2. Link the program
	error = Link();
	if (error.Failed())
	{
		error.Uncheck();
		return error;
	}

	// 3. Validate the program
	error = Validate();
	if (error.Failed())
	{
		error.Uncheck();
		return error;
	}

	// 4. Generate uniforms
	GenerateUniforms();

	m_isLinked = true;
	return CMG_ERROR_SUCCESS;
}


//-----------------------------------------------------------------------------
// Static methods
//-----------------------------------------------------------------------------

Error Shader::LoadShader(Shader*& outShader,
	const Path& vertexPath, const Path& fragmentPath)
{
	// Load the code
	String vertexCode, fragmentCode;
	Error error = File::OpenAndGetContents(vertexPath, vertexCode);
	if (error.Failed())
		return error.Uncheck();
	error = File::OpenAndGetContents(fragmentPath, fragmentCode);
	if (error.Failed())
		return error.Uncheck();

	// Add the stages
	outShader = new Shader();
	error = outShader->AddStage(ShaderType::k_vertex_shader,
		vertexCode, vertexPath);
	if (error.Failed())
		return error.Uncheck();
	error = outShader->AddStage(ShaderType::k_fragment_shader,
		fragmentCode, fragmentPath);
	if (error.Failed())
		return error.Uncheck();

	// Compile
	return outShader->CompileAndLink();
}

Error Shader::LoadComputeShader(Shader*& outShader, const Path& path)
{
	Array<Path> paths;
	return LoadComputeShader(outShader, path, paths);
}

Error Shader::LoadComputeShader(Shader*& outShader, const Path& path, const Array<Path>& paths)
{
	// Load the code
	String code;
	Error error = File::OpenAndGetContents(path, code);
	if (error.Failed())
		return error.Uncheck();

	// Add the stages
	outShader = new Shader();
	error = outShader->AddStage(ShaderType::k_compute_shader, code, path);
	if (error.Failed())
		return error.Uncheck();

	// Compile
	return outShader->CompileAndLink();
}

Error Shader::UnloadImpl()
{
	DeleteGLProgram();
	m_isLinked = false;
	m_uniforms.clear();
	return CMG_ERROR_SUCCESS;
}

Error Shader::LoadImpl()
{
	return LoadImpl(m_loadArgs);
}

Error Shader::LoadImpl(const ShaderLoadArgs& args)
{
	Error error;

	DeleteGLProgram();
	CreateGLProgram();

	m_loadArgs = args;

	for (auto it : args.stages)
	{
		ShaderType stageType = it.first;
		String name = it.second;
		CMG_LOG_DEBUG() << "  - Compiling " << GetShaderStageName(stageType)
			<< " shader: " << name;

		// Load the code from the text file
		File file;
		error = GetResourceLoader()->OpenResourceFile(
			file, name, FileAccess::READ, FileType::TEXT);
		if (error.Failed())
			return error.Uncheck();
		String code;
		error = file.GetContents(code);
		if (error.Failed())
			return error.Uncheck();

		// Add the stage to the shader
		error = AddStage(stageType, code, name);
		if (error.Failed())
			return error.Uncheck();
	}

	return CompileAndLink();
}


//-----------------------------------------------------------------------------
// Private methods
//-----------------------------------------------------------------------------

void Shader::CreateGLProgram()
{
	if (m_glProgram == 0)
		m_glProgram = glCreateProgram();
}

void Shader::DeleteGLProgram()
{
	if (m_glProgram != 0)
	{
		glDeleteProgram(m_glProgram);
		m_glProgram = 0;
	}
	for (int i = 0; i < (int) ShaderType::k_count; ++i)
		m_stages[i].Unload();
}

Error Shader::Compile(const Array<Path>& paths)
{
	Array<String> compileErrors;
	Error compileError;

	// Pre-process and compile all shader stages
	for (uint32 i = 0; i < (int) ShaderType::k_count; ++i)
	{
		ShaderStage& stage = m_stages[i];
		if (stage.m_isEnabled)
		{
			compileError = PreprocessStage(stage, paths);
			if (compileError.Passed())
				compileError = CompileStage(stage);
			if (compileError.Failed())
				compileErrors.push_back(compileError.GetText());
		}
	}

	// Check if there were errors
	if (compileErrors.size() > 0)
	{
		// Combine all errors into one message
		String errorMessage = "There were shader compile errors:\n\n";
		for (uint32 i = 0; i < compileErrors.size(); ++i)
		{
			if (i > 0)
				errorMessage += "\n";
			errorMessage += compileErrors[i];
		}
		return CMG_ERROR_MSG(CommonErrorTypes::k_failure, errorMessage);
	}

	return CMG_ERROR_SUCCESS;
}

Error Shader::PreprocessStage(ShaderStage& stage, const Array<Path>& paths)
{
	CMG_ASSERT(stage.m_isEnabled);

	std::regex regexInclude("\\s*#\\s*include\\s+[<\"](.*)[>\"]");

	String oldCode = stage.m_code;
	std::sregex_iterator matchesBegin = std::sregex_iterator(
		oldCode.begin(), oldCode.end(), regexInclude);
	std::sregex_iterator matchesEnd = std::sregex_iterator();
	Path stageFileDir = stage.m_path.GetParent();
	Error error;
	uint32 offset = 0;

	for (std::sregex_iterator it = matchesBegin; it != matchesEnd; ++it)
	{
		const std::smatch& match = *it;
		std::string match_str = match.str();
		if (match.size() > 1)
		{
			Path includePath = Path(match[1].str());
			Path resolvedPath = stageFileDir / includePath;
			if (!resolvedPath.Exists())
				resolvedPath = Path::ResolvePath(includePath, paths);
			if (resolvedPath.FileExists())
			{
				String includeCode;
				error = File::OpenAndGetContents(resolvedPath, includeCode);
				if (error.Failed())
					return error.Uncheck();
				stage.m_code = stage.m_code.replace(
					match.position() + offset, match.length(), includeCode);
				offset += includeCode.length() - match.length();
			}
			else
			{
				return CMG_ERROR_FAILURE;
			}
		}
	} 

	// Send the shader's source code to OpenGL
	const GLchar* str[1] = { stage.m_code.c_str() };
	GLint lengths[1] = { (GLint) stage.m_code.length() };
	glShaderSource(stage.m_glShaderName, 1, str, lengths);

	return CMG_ERROR_SUCCESS;
}

Error Shader::CompileStage(ShaderStage& stage)
{
	CMG_ASSERT(stage.m_isEnabled);

	// Compile the shader
	glCompileShader(stage.m_glShaderName);

	// Check for errors
	GLint isStatusOK;
	glGetShaderiv(stage.m_glShaderName, GL_COMPILE_STATUS, &isStatusOK);
	if (isStatusOK == GL_FALSE)
	{
		// Get the shader compile error message
		GLchar errorMsg[1024] = { 0 };
		glGetShaderInfoLog(stage.m_glShaderName,
			sizeof(errorMsg), NULL, errorMsg);

		// Format the error message
		String errorMessage = errorMsg;
		errorMessage = "\n" + errorMessage;
		String toReplace = "\n" + stage.m_path.ToString() + "(";

		// Insert the shader file name before each error
		size_t index = 0;
		while (true)
		{
			// Locate the substring to replace
			index = errorMessage.find("\n0(", index);
			if (index == String::npos)
				break;

			// Make the replacement
			errorMessage.replace(index, 3, toReplace);

			// Advance index forward so the next iteration doesn't pick it up
			// as well
			index += toReplace.length();
		}
		errorMessage.erase(errorMessage.begin());

		return CMG_ERROR_MSG(CommonErrorTypes::k_failure, errorMessage);
	}

	return CMG_ERROR_SUCCESS;
}

Error Shader::Link()
{
	// Link the program
	glLinkProgram(m_glProgram);

	// Check for errors
	GLint isStatusOK;
	glGetProgramiv(m_glProgram, GL_LINK_STATUS, &isStatusOK);
	if (isStatusOK == GL_FALSE)
	{
		// Get the error message
		GLchar errorMsg[1024] = { 0 };
		glGetProgramInfoLog(m_glProgram, sizeof(errorMsg), NULL, errorMsg);
		String errorMessage = "Error linking shader:\n\n" + String(errorMsg) + "\n";
		return CMG_ERROR_MSG(CommonErrorTypes::k_failure, errorMessage);
	}

	return CMG_ERROR_SUCCESS;
}

Error Shader::Validate()
{
	// Validate the program
	glValidateProgram(m_glProgram);

	// Check for errors
	GLint isStatusOK;
	glGetProgramiv(m_glProgram, GL_VALIDATE_STATUS, &isStatusOK);
	if (isStatusOK == GL_FALSE)
	{
		// Get the error message
		GLchar errorMsg[1024] = { 0 };
		glGetProgramInfoLog(m_glProgram, sizeof(errorMsg), NULL, errorMsg);
		String errorMessage = "Error validation failed:\n\n" + String(errorMsg) + "\n";
		return CMG_ERROR_MSG(CommonErrorTypes::k_failure, errorMessage);
	}

	return CMG_ERROR_SUCCESS;
}

void Shader::GenerateUniforms()
{
	// Find the number of uniforms and the max uniform name length.
	int maxNameLength;
	int numUniforms;
	glGetProgramiv(m_glProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
	glGetProgramiv(m_glProgram, GL_ACTIVE_UNIFORMS, &numUniforms);
	m_uniforms.resize(numUniforms);
	GLchar* uniformName = new char[maxNameLength + 1];

	int samplerSlot = 0;

	// Create a list of all shader parameters.
	for (int i = 0; i < numUniforms; ++i)
	{
		Uniform& uniform = m_uniforms[i];
		uniform.m_name = new char[maxNameLength];

		GLint size;
		GLenum type;
		glGetActiveUniform(m_glProgram, (GLuint) i,
			maxNameLength, NULL, &size, &type, uniformName);

		uniform.m_name = uniformName;
		uniform.m_location = glGetUniformLocation(m_glProgram, uniformName);

		switch (type)
		{
		case GL_BOOL:
			uniform.m_type = UniformType::k_boolean;
			break;
		case GL_INT:
			uniform.m_type = UniformType::k_integer;
			break;
		case GL_INT_VEC2:
			uniform.m_type = UniformType::k_ivec2;
			break;
		case GL_INT_VEC3:
			uniform.m_type = UniformType::k_ivec3;
			break;
		case GL_INT_VEC4:
			uniform.m_type = UniformType::k_ivec4;
			break;
		case GL_UNSIGNED_INT:
			uniform.m_type = UniformType::k_unsigned_int;
			break;
		case GL_UNSIGNED_INT_VEC2:
			uniform.m_type = UniformType::k_uvec2;
			break;
		case GL_UNSIGNED_INT_VEC3:
			uniform.m_type = UniformType::k_uvec3;
			break;
		case GL_UNSIGNED_INT_VEC4:
			uniform.m_type = UniformType::k_uvec4;
			break;
		case GL_FLOAT:
			uniform.m_type = UniformType::k_float;
			break;
		case GL_FLOAT_VEC2:
			uniform.m_type = UniformType::k_vec2;
			break;
		case GL_FLOAT_VEC3:
			uniform.m_type = UniformType::k_vec3;
			break;
		case GL_FLOAT_VEC4:
			uniform.m_type = UniformType::k_vec4;
			break;
		case GL_FLOAT_MAT4:
			uniform.m_type = UniformType::k_matrix;
			break;
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
			uniform.m_type = UniformType::k_texture;
			uniform.m_samplerSlot = samplerSlot++;
			break;
		default:
			fprintf(stderr, "Warning: unsupported GLSL uniform type for '%s'\n", uniformName);
			//CMG_ASSERT_FALSE("Unsupported uniform type");
			uniform.m_type = UniformType::k_unknown;
			break;
		}
	}

	delete[] uniformName;
}

