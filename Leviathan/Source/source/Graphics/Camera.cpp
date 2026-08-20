#include "Graphics/Camera.h"

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "Maths/Alias.h"

using glm::vec3;

namespace Leviathan
{
	Camera::Camera(const float fov, const float near, const float far) :
		m_fovY{ fov }, m_nearPlane{ near }, m_farPlane{ far }
	{}

	mat4 Camera::Projection() const
	{
		int32 w, h;
		glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);

		const mat4 perspective = glm::perspective(
			glm::radians(m_fovY), static_cast<float>(w) / static_cast<float>(h), m_nearPlane, m_farPlane
		);

		return perspective;
	}

	mat4 Camera::ProjectionView() const
	{
		return Projection() * View();
	}
}
