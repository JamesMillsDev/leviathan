#pragma once

#include <glm/mat4x4.hpp>

using glm::mat4;

namespace Leviathan
{
	struct TransformComponent
	{
		mat4 transform;
	};
}
