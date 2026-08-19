// ReSharper disable CppMemberFunctionMayBeStatic
#include "Graphics/Material.h"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

namespace Leviathan
{
	class Texture
	{};

	Material::Material(Shader* shader)
		: m_shader{ shader }, m_vpLoc{ FindUniform("vp") }, m_modelLoc{ FindUniform("model") }
	{

	}

	bool Material::Bind() const
	{
		return m_shader->Bind();
	}

	void Material::Set(const string& name, const int32 value)
	{
		Set(FindUniform(name), value);
	}

	void Material::Set(const string& name, const float value)
	{
		Set(FindUniform(name), value);
	}

	void Material::Set(const string& name, const vec2 value)
	{
		Set(FindUniform(name), value);
	}

	void Material::Set(const string& name, const vec3 value)
	{
		Set(FindUniform(name), value);
	}

	void Material::Set(const string& name, const vec4 value)
	{
		Set(FindUniform(name), value);
	}

	void Material::Set(const string& name, const mat3& value)
	{
		Set(FindUniform(name), value);
	}

	void Material::Set(const string& name, const mat4& value)
	{
		Set(FindUniform(name), value);
	}

	void Material::Set(const string& name, Texture value)
	{
		// TODO:
	}

	void Material::Set(const string& name, const Color& value)
	{
		Set(FindUniform(name), value);
	}

	void Material::Set(const string& name, const int32* value, const int32 count)
	{
		Set(FindUniform(name), value, count);
	}

	void Material::Set(const string& name, const float* value, const int32 count)
	{
		Set(FindUniform(name), value, count);
	}

	void Material::Set(const string& name, vec2* value, const int32 count)
	{
		Set(FindUniform(name), value, count);
	}

	void Material::Set(const string& name, vec3* value, const int32 count)
	{
		Set(FindUniform(name), value, count);
	}

	void Material::Set(const string& name, vec4* value, const int32 count)
	{
		Set(FindUniform(name), value, count);
	}

	void Material::Set(const string& name, mat3* value, const int32 count)
	{
		Set(FindUniform(name), value, count);
	}

	void Material::Set(const string& name, mat4* value, const int32 count)
	{
		Set(FindUniform(name), value, count);
	}

	void Material::Set(const int32 id, const int32 value)
	{
		if (id == -1)
		{
			return;
		}

		glUniform1i(id, value);
	}

	void Material::Set(const int32 id, const float value)
	{
		if (id == -1)
		{
			return;
		}

		glUniform1f(id, value);
	}

	void Material::Set(const int32 id, vec2 value)
	{
		if (id == -1)
		{
			return;
		}

		glUniform2fv(id, 1, glm::value_ptr(value));
	}

	void Material::Set(const int32 id, vec3 value)
	{
		if (id == -1)
		{
			return;
		}

		glUniform3fv(id, 1, glm::value_ptr(value));
	}

	void Material::Set(const int32 id, vec4 value)
	{
		if (id == -1)
		{
			return;
		}

		glUniform4fv(id, 1, glm::value_ptr(value));
	}

	void Material::Set(const int32 id, mat3 value)
	{
		if (id == -1)
		{
			return;
		}

		glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(value[0]));
	}

	void Material::Set(const int32 id, mat4 value)
	{
		if (id == -1)
		{
			return;
		}

		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(value[0]));
	}

	void Material::Set(int32 id, Texture value)
	{
		// TODO
	}

	void Material::Set(const int32 id, const Color& value)
	{
		if (id == -1)
		{
			return;
		}

		glUniform4fv(id, 1, &static_cast<vec4>(value)[0]);
	}

	void Material::Set(const int32 id, const int32* value, const int32 count)
	{
		if (id == -1)
		{
			return;
		}

		glUniform1iv(id, count, value);
	}

	void Material::Set(const int32 id, const float* value, const int32 count)
	{
		if (id == -1)
		{
			return;
		}

		glUniform1fv(id, count, value);
	}

	void Material::Set(const int32 id, vec2* value, const int32 count)
	{
		if (id == -1)
		{
			return;
		}

		glUniform2fv(id, count, glm::value_ptr(value[0]));
	}

	void Material::Set(const int32 id, vec3* value, const int32 count)
	{
		if (id == -1)
		{
			return;
		}

		glUniform3fv(id, count, glm::value_ptr(value[0]));
	}

	void Material::Set(const int32 id, vec4* value, const int32 count)
	{
		if (id == -1)
		{
			return;
		}

		glUniform4fv(id, count, glm::value_ptr(value[0]));
	}

	void Material::Set(const int32 id, mat3* value, const int32 count)
	{
		if (id == -1)
		{
			return;
		}

		glUniformMatrix3fv(id, count, GL_FALSE, glm::value_ptr(value[0]));
	}

	void Material::Set(const int32 id, mat4* value, const int32 count)
	{
		if (id == -1)
		{
			return;
		}

		glUniformMatrix4fv(id, count, GL_FALSE, glm::value_ptr(value[0]));
	}

	int32 Material::FindUniform(const string& name) const
	{
		return glGetUniformLocation(m_shader->m_handle, name.c_str());
	}
}
