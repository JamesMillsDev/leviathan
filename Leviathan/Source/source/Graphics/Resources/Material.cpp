// ReSharper disable CppMemberFunctionMayBeStatic
#include "Graphics/Resources/Material.h"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include "Graphics/Resources/Textures/Texture.h"

namespace Leviathan
{
	Material::Material(Shader* shader) :
		m_shader{ shader }, m_vpLoc{ FindUniform("pv") }, m_modelLoc{ FindUniform("model") },
		m_normalMatrixLoc{ FindUniform("normMatrix") },
		m_cameraLocationLoc{ FindUniform("cameraLocation") }, m_lastTextureSlot{ 0 }, m_isLit{ true }
	{}

	const TMap<string, MaterialProperty>& Material::GetMaterialProperties()
	{
		return m_properties;
	}

	const TMap<string, MaterialTextureProperty>& Material::GetMaterialTextureProperties()
	{
		return m_textures;
	}

	void Material::SetMaterialProperty(const string& id, EMaterialPropertyType type, MaterialPropertyUnion value, bool overrideValue)
	{
		if (m_properties.ContainsKey(id))
		{
			if (overrideValue)
			{
				m_properties[id].type = type;
				m_properties[id].value = value;
				m_properties[id].loc = FindUniform(id);
			}

			return;
		}

		MaterialProperty property =
		{
			.loc = FindUniform(id),
			.type = type,
			.value = value
		};

		m_properties.Add(id, property);
	}

	void Material::SetTexture(const string& id, Texture* texture, bool overrideValue)
	{
		if (m_textures.ContainsKey(id))
		{
			if (overrideValue)
			{
				m_textures[id].loc = FindUniform(id);
				m_textures[id].texture = texture;
			}

			return;
		}

		m_textures.Add(id, { .loc = FindUniform(id), .texture = texture, .slot = m_lastTextureSlot++ });
	}

	void Material::SetLitState(bool isLit)
	{
		m_isLit = isLit;
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

	void Material::Set(const string& name, const Texture& value, const int32 slot)
	{
		Set(FindUniform(name), value, slot);
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

	void Material::Set(const int32 id, const Texture& value, const int32 slot)
	{
		if (id == -1)
		{
			return;
		}

		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, value.Handle());
		glUniform1i(id, slot);
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

	void Material::SetMaterialProperties(uint32 shadowMapHandle, bool useShadowMapping, int32 maxIndex)
	{
		for (TMapEntry<string, MaterialProperty>* property : m_properties)
		{
			switch (auto& [loc, type, value] = property->Value(); type)
			{
				case EMaterialPropertyType::Int:
				{
					Set(loc, value.iValue);
					break;
				}
				case EMaterialPropertyType::Float:
				{
					Set(loc, value.fValue);
					break;
				}
				case EMaterialPropertyType::Vec2:
				{
					Set(loc, value.v2Value);
					break;
				}
				case EMaterialPropertyType::Vec3:
				{
					Set(loc, value.v3Value);
					break;
				}
				case EMaterialPropertyType::Vec4:
				{
					Set(loc, value.v4Value);
					break;
				}
				case EMaterialPropertyType::Mat3:
				{
					Set(loc, value.m3Value);
					break;
				}
				case EMaterialPropertyType::Mat4:
				{
					Set(loc, value.m4Value);
					break;
				}
			}
		}

		int32 maxSlot = maxIndex;
		for (TMapEntry<string, MaterialTextureProperty>* textureProperty : m_textures)
		{
			auto& [loc, texture, slot] = textureProperty->Value();
			maxSlot = std::max(maxSlot, slot);
			Set(loc, *texture, maxSlot);
		}

		if (m_isLit && useShadowMapping)
		{
			glActiveTexture(GL_TEXTURE0 + maxSlot + 1);
			glBindTexture(GL_TEXTURE_2D, shadowMapHandle);
			Set("shadows.map", maxSlot + 1);
			Set("shadows.bias", .005f);
			Set("shadows.texelSize", .5f);
			Set("shadows.samples", 2);
		}
	}

	void Material::CopyMaterialProperties(Material* material)
	{
		for (TMapEntry<string, MaterialProperty>* property : material->m_properties)
		{
			SetMaterialProperty(property->Key(), property->Value().type, property->Value().value, true);
		}

		for (TMapEntry<string, MaterialTextureProperty>* textureProperty : material->m_textures)
		{
			SetTexture(textureProperty->Key(), textureProperty->Value().texture, true);
		}
	}

	bool Material::Bind() const
	{
		return m_shader->Bind();
	}

	void Material::ClearMaterialProperties()
	{
		m_properties.Clear();
		m_textures.Clear();
		m_lastTextureSlot = 0;
	}

	int32 Material::FindUniform(const string& name) const
	{
		return glGetUniformLocation(m_shader->m_handle, name.c_str());
	}
}
