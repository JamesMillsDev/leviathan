#pragma once

#include <functional>
#include <string>

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Shader.h"
#include "Maths/Alias.h"
#include "Maths/Color.h"

using std::string;

using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace Leviathan
{
	class Texture;

	class Material
	{
		friend class Renderer;

	private:
		Shader* m_shader;
		int32 m_vpLoc;
		int32 m_modelLoc;

	public:
		explicit Material(Shader* shader);

	public:
		bool Bind() const;

		void Set(const string& name, int32 value);
		void Set(const string& name, float value);
		void Set(const string& name, vec2 value);
		void Set(const string& name, vec3 value);
		void Set(const string& name, vec4 value);
		void Set(const string& name, const mat3& value);
		void Set(const string& name, const mat4& value);
		void Set(const string& name, Texture value);
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
		void Set(int32 id, Texture value);
		void Set(int32 id, const Color& value);
		void Set(int32 id, const int32* value, int32 count);
		void Set(int32 id, const float* value, int32 count);
		void Set(int32 id, vec2* value, int32 count);
		void Set(int32 id, vec3* value, int32 count);
		void Set(int32 id, vec4* value, int32 count);
		void Set(int32 id, mat3* value, int32 count);
		void Set(int32 id, mat4* value, int32 count);

		int32 FindUniform(const string& name) const;

	};
}
