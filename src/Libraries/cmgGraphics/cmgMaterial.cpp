#include "cmgMaterial.h"


Material::Material()
{
}

Shader::sptr Material::GetShader() const
{
	return m_shader;
}

const UniformValue& Material::GetUniform(const String & name) const
{
	return m_uniforms.at(name);
}

void Material::SetShader(Shader::sptr shader)
{
	m_shader = shader;
}

