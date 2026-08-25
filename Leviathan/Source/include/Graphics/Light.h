#pragma once

#include <glm/mat4x4.hpp>

#include "Maths/Color.h"

namespace Leviathan {
	class Material;
}

using glm::mat4;

namespace Leviathan
{
	enum class ELightType : uint8
	{
		Directional,
		Point,
		Spot,
	};

	class Light
	{
	public:
		mat4 transform;
		Color color;
		float intensity;
		ELightType type;

	public:
		Light();

	public:
		void SetMaterialProperties(Material* material, int32 index);

	};
}
