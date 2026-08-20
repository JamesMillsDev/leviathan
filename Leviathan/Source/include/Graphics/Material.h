#pragma once

#include <string>

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Shader.h"
#include "Maths/Alias.h"
#include "Maths/Color.h"
#include "Utility/Collections/TMap.h"

using std::string;

using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace Leviathan
{
	class Texture;

	enum class EMaterialPropertyType
	{
		Int,
		Float,
		Vec2,
		Vec3,
		Vec4,
		Mat3,
		Mat4
	};

	union MaterialPropertyUnion
	{
		int32 iValue;
		float fValue;
		vec2 v2Value;
		vec3 v3Value;
		vec4 v4Value;
		mat3 m3Value;
		mat4 m4Value;
	};

	struct MaterialProperty
	{
		int32 loc;
		EMaterialPropertyType type;
		MaterialPropertyUnion value;
	};

	struct MaterialTextureProperty
	{
		int32 loc;
		Texture* texture;
		int32 slot;
	};

	class Material
	{
		friend class Renderer;

	private:
		Shader* m_shader;
		int32 m_vpLoc;
		int32 m_modelLoc;
		int32 m_normalMatrixLoc;
		int32 m_cameraLocationLoc;
		int32 m_lastTextureSlot;

		TMap<string, MaterialProperty> m_properties;
		TMap<string, MaterialTextureProperty> m_textures;

	public:
		explicit Material(Shader* shader);

	public:
		void SetMaterialProperty(const string& id, EMaterialPropertyType type, MaterialPropertyUnion value);
		void SetTexture(const string& id, Texture* texture);

		void Set(const string& name, int32 value);
		void Set(const string& name, float value);
		void Set(const string& name, vec2 value);
		void Set(const string& name, vec3 value);
		void Set(const string& name, vec4 value);
		void Set(const string& name, const mat3& value);
		void Set(const string& name, const mat4& value);
		void Set(const string& name, const Texture& value, int32 slot);
		void Set(const string& name, const Color& value);
		void Set(const string& name, const int32* value, int32 count);
		void Set(const string& name, const float* value, int32 count);
		void Set(const string& name, vec2* value, int32 count);
		void Set(const string& name, vec3* value, int32 count);
		void Set(const string& name, vec4* value, int32 count);
		void Set(const string& name, mat3* value, int32 count);
		void Set(const string& name, mat4* value, int32 count);

		void Set(int32 id, int32 value);
		void Set(int32 id, float value);
		void Set(int32 id, vec2 value);
		void Set(int32 id, vec3 value);
		void Set(int32 id, vec4 value);
		void Set(int32 id, mat3 value);
		void Set(int32 id, mat4 value);
		void Set(int32 id, const Texture& value, int32 slot);
		void Set(int32 id, const Color& value);
		void Set(int32 id, const int32* value, int32 count);
		void Set(int32 id, const float* value, int32 count);
		void Set(int32 id, vec2* value, int32 count);
		void Set(int32 id, vec3* value, int32 count);
		void Set(int32 id, vec4* value, int32 count);
		void Set(int32 id, mat3* value, int32 count);
		void Set(int32 id, mat4* value, int32 count);
		
	private:
		void SetMaterialProperties();
		bool Bind() const;

		int32 FindUniform(const string& name) const;

	};
}
