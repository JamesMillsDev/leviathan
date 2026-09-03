#include "Graphics/Light.h"

#include <format>

#include "Graphics/Resources/Material.h"

namespace Leviathan
{
	Light::Light() :
		transform{ mat4{ 1.f } }, color{ 1.f, 1.f, 1.f, 1.f }, intensity{ 1.f },
		type{ ELightType::Directional }
	{}

	void Light::SetMaterialProperties(Material* material, int32 index)
	{
		material->Set(std::format("lights[{}].type", index), static_cast<int32>(type));
		material->Set(std::format("lights[{}].direction", index), vec3{ glm::normalize(transform[2]) });
		material->Set(std::format("lights[{}].color", index), vec3{ static_cast<vec4>(color) * intensity });
		material->Set(std::format("lights[{}].location", index), vec3{ transform[3] }); 
	}
}
