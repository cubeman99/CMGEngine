#ifndef _CMG_GRAPHICS_MATERIAL_H_
#define _CMG_GRAPHICS_MATERIAL_H_

#include <cmgMath/types/cmgMatrix3f.h>
#include <cmgMath/types/cmgMatrix4f.h>
#include <cmgMath/types/cmgVector3.h>
#include <cmgGraphics/cmgShader.h>
#include <cmgGraphics/cmgTexture.h>


struct UniformValue
{
	UniformValue() {}
	~UniformValue() {}
	UniformValue(Texture::sptr texture) { this->texture = texture; type = UniformType::k_texture; }
	UniformValue(float32 value) { this->value.float32_value = value; type = UniformType::k_float; }
	UniformValue(uint32 value) { this->value.uint32_value = value; type = UniformType::k_integer; }
	UniformValue(const Matrix3f& mat3) { this->value.mat4 = Matrix4f(mat3); type = UniformType::k_matrix; }
	UniformValue(const Matrix4f& mat4) { this->value.mat4 = mat4; type = UniformType::k_matrix; }
	UniformValue(const Vector2f& vec2) { this->value.vec2 = vec2; type = UniformType::k_vec2; }
	UniformValue(const Vector3f& vec3) { this->value.vec3 = vec3; type = UniformType::k_vec3; }
	UniformValue(const Vector4f& vec4) { this->value.vec4 = vec4; type = UniformType::k_vec4; }
	UniformValue(const Color& color) { this->value.vec4 = color.ToVector4f(); type = UniformType::k_vec4; }
	UniformValue(const Vector2ui& uvec2) { this->value.uvec2 = uvec2; type = UniformType::k_uvec2; }
	UniformValue(const Vector3ui& uvec3) { this->value.uvec3 = uvec3; type = UniformType::k_uvec3; }
	UniformValue(const Vector2i& ivec2) { this->value.ivec2 = ivec2; type = UniformType::k_ivec2; }
	UniformValue(const Vector3i& ivec3) { this->value.ivec3 = ivec3; type = UniformType::k_ivec3; }

	union Value
	{
		Value()
		{
			memset(this, 0, sizeof(Value));
		}
		Value(const Value& copy)
		{
			*this = copy;
		}
		~Value() {}
		void operator=(const Value& copy)
		{
			memcpy(this, &copy, sizeof(Value));
		}

		float32 float32_value;
		uint32 uint32_value;
		Vector2f vec2;
		Vector3f vec3;
		Vector4f vec4;
		Vector2i ivec2;
		Vector3i ivec3;
		Vector2ui uvec2;
		Vector3ui uvec3;
		Matrix3f mat3;
		Matrix4f mat4;
	};

	Value value;
	Texture::sptr texture;
	UniformType type;
};


class Material : public cmg::ResourceImpl<Material>
{
public:
	using sptr = cmg::shared_ptr<Material>;
	using uniform_map = Map<String, UniformValue>;

public:
	Material();

	Shader::sptr GetShader() const;
	const UniformValue& GetUniform(const String& name) const;

	uniform_map::iterator uniforms_begin() { return m_uniforms.begin(); }
	uniform_map::iterator uniforms_end() { return m_uniforms.end(); }

	void SetShader(Shader::sptr shader);

	template <typename T>
	void SetUniform(const String& name, T value);

private:
	// Prevent copying
	Material(Material& other) {}
	void operator=(Material& other) {}

private:
	Shader::sptr m_shader;
	uniform_map m_uniforms;
};


template <typename T>
void Material::SetUniform(const String& name, T value)
{
	m_uniforms[name] = UniformValue(value);
}



#endif // _CMG_GRAPHICS_MATERIAL_H_