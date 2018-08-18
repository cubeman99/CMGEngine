#include "cmgShader.h"
#include <cmgGraphics/cmgOpenGLIncludes.h>
#include <cmgGraphics/render/cmgSampler.h>
#include <cmgGraphics/cmgTexture.h>
#include <cmgGraphics/render/cmgRenderDevice.h>
#include <assert.h>


//-----------------------------------------------------------------------------
// Constructor & destructor
//-----------------------------------------------------------------------------

Shader::Shader(OpenGLRenderDevice* device)
	: m_isLinked(false)
	, m_device(device)
{
	// Create the shader program
	m_glProgram = glCreateProgram();
	for (int i = 0; i < (int) ShaderType::k_count; ++i)
		m_glShaderStages[i] = 0;
}

Shader::~Shader()
{
	// Delete the shader program.
	glDeleteProgram(m_glProgram);

	// Delete the shader stages.
	for (int i = 0; i < (int) ShaderType::k_count; ++i)
	{
		if (m_glShaderStages != 0)
			glDeleteShader(m_glShaderStages[i]);
	}
}



//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------

bool Shader::IsLinked() const
{
	return m_isLinked;
}

unsigned int Shader::GetNumUniforms() const
{
	return m_uniforms.size();
}

const Uniform& Shader::GetUniform(unsigned int index) const
{
	return m_uniforms[index];
}

const Uniform* Shader::GetUniform(const String& name) const
{
	for (unsigned int i = 0; i < m_uniforms.size(); ++i)
	{
		if (m_uniforms[i].m_name == name)
			return &m_uniforms[i];
	}
	return nullptr;
}

int Shader::GetUniformLocation(const String& name) const
{
	for (unsigned int i = 0; i < m_uniforms.size(); ++i)
	{
		if (m_uniforms[i].m_name == name)
			return m_uniforms[i].GetLocation();
	}
	return -1;
}

bool Shader::GetUniformLocation(const String& name, int& outUniformLocation) const
{
	for (unsigned int i = 0; i < m_uniforms.size(); ++i)
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

Error Shader::AddStage(ShaderType type, const String& code, const String& fileName)
{
	GLuint shaderGL = 0;

	// Create the correct shader object.
	if (type == ShaderType::k_vertex_shader)
		shaderGL = glCreateShader(GL_VERTEX_SHADER);
	else if (type == ShaderType::k_fragment_shader)
		shaderGL = glCreateShader(GL_FRAGMENT_SHADER);
	else if (type == ShaderType::k_geometry_shader)
		shaderGL = glCreateShader(GL_GEOMETRY_SHADER);
	else if (type == ShaderType::k_compute_shader)
		shaderGL = glCreateShader(GL_COMPUTE_SHADER);
	else
	{
		CMG_ASSERT_FALSE("Invalid shader stage type.");
		return CMG_ERROR_FAILURE;
	}

	// Set the shader's source code.
	const GLchar* str[1] = { code.c_str() };
	GLint lengths[1] = { code.length() };
	glShaderSource(shaderGL, 1, str, lengths);

	// Attach the shader stage to the shader program.
	glAttachShader(m_glProgram, shaderGL);
	m_glShaderStages[(int) type] = shaderGL;
	m_shaderStageFileNames[(int) type] = fileName;
	m_isLinked = false;
	return CMG_ERROR_SUCCESS;
}

Error Shader::CompileAndLink()
{
	Error error;

	// 1. Compile the stages.
	error = Compile();
	if (error.Failed())
	{
		error.Uncheck();
		return error;
	}

	// 2. Link the program.
	error = Link();
	if (error.Failed())
	{
		error.Uncheck();
		return error;
	}

	// 3. Validate the program.
	error = Validate();
	if (error.Failed())
	{
		error.Uncheck();
		return error;
	}

	// 4. Generate uniforms.
	GenerateUniforms();

	m_isLinked = true;
	return CMG_ERROR_SUCCESS;
}

Error Shader::SetSampler(const String& samplerName,
	Texture* texture, Sampler* sampler, uint32 slot)
{
	return m_device->SetShaderSampler(this, samplerName, texture, sampler,
		slot);
}


//-----------------------------------------------------------------------------
// Private methods
//-----------------------------------------------------------------------------

Error Shader::Compile()
{
	Array<String> compileErrors;
	Error compileError;

	// Compile all shader stages.
	for (unsigned int i = 0; i < (int) ShaderType::k_count; ++i)
	{
		if (m_glShaderStages[i] != 0)
		{
			compileError = CompileStage((ShaderType) i);
			if (compileError.Failed())
				compileErrors.push_back(compileError.GetText());
		}
	}

	// Check if there were compile errors.
	if (compileErrors.size() > 0)
	{
		// Combine all errors into one message.
		String errorMessage = "There were shader compile errors:\n\n";
		for (unsigned int i = 0; i < compileErrors.size(); ++i)
		{
			if (i > 0)
				errorMessage += "\n";
			errorMessage += compileErrors[i];
		}
		return CMG_ERROR_MSG(CommonErrorTypes::k_failure, errorMessage);
	}

	return CMG_ERROR_SUCCESS;
}

Error Shader::CompileStage(ShaderType stage)
{
	GLuint shaderGL = m_glShaderStages[(int) stage];
	CMG_ASSERT(shaderGL != 0);

	// Compile the shader.
	glCompileShader(shaderGL);

	// Check for errors.
	GLint isStatusOK;
	glGetShaderiv(shaderGL, GL_COMPILE_STATUS, &isStatusOK);
	if (isStatusOK == GL_FALSE)
	{
		// Get the shader compile error message.
		GLchar errorMsg[1024] = { 0 };
		glGetShaderInfoLog(shaderGL, sizeof(errorMsg), NULL, errorMsg);

		// Format the error message.
		String errorMessage = errorMsg;
		errorMessage = "\n" + errorMessage;
		String toReplace = "\n" + m_shaderStageFileNames[(int) stage] + "(";

		// Insert the shader file name before each error.
		size_t index = 0;
		while (true)
		{
			// Locate the substring to replace.
			index = errorMessage.find("\n0(", index);
			if (index == String::npos)
				break;

			// Make the replacement.
			errorMessage.replace(index, 3, toReplace);

			// Advance index forward so the next iteration doesn't pick it up as well.
			index += toReplace.length();
		}
		errorMessage.erase(errorMessage.begin());

		return CMG_ERROR_MSG(CommonErrorTypes::k_failure, errorMessage);
	}

	return CMG_ERROR_SUCCESS;
}

Error Shader::Link()
{
	// Link the program.
	glLinkProgram(m_glProgram);

	// Check for errors.
	GLint isStatusOK;
	glGetProgramiv(m_glProgram, GL_LINK_STATUS, &isStatusOK);
	if (isStatusOK == GL_FALSE)
	{
		// Get the error message.
		GLchar errorMsg[1024] = { 0 };
		glGetProgramInfoLog(m_glProgram, sizeof(errorMsg), NULL, errorMsg);
		String errorMessage = "Error linking shader:\n\n" + String(errorMsg) + "\n";
		return CMG_ERROR_MSG(CommonErrorTypes::k_failure, errorMessage);
	}

	return CMG_ERROR_SUCCESS;
}

Error Shader::Validate()
{
	// Validate the program.
	glValidateProgram(m_glProgram);

	// Check for errors.
	GLint isStatusOK;
	glGetProgramiv(m_glProgram, GL_VALIDATE_STATUS, &isStatusOK);
	if (isStatusOK == GL_FALSE)
	{
		// Get the error message.
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
		uniform.m_location = i;
		uniform.m_name = new char[maxNameLength];

		GLint size;
		GLenum type;
		glGetActiveUniform(m_glProgram, (GLuint) i,
			maxNameLength, NULL, &size, &type, uniformName);

		uniform.m_name = uniformName;

		switch (type)
		{
		case GL_BOOL:
			uniform.m_type = UniformType::k_boolean;
			break;
		case GL_INT:
			uniform.m_type = UniformType::k_integer;
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
			//fprintf(stderr, "Warning: unsupported GLSL uniform type for '%s'\n", uniformName);
			CMG_ASSERT_FALSE("Unsupported uniform type");
			break;
		}
	}

	delete[] uniformName;
}

