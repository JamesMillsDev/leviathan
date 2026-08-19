#include "Graphics/Camera.h"

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Maths/Alias.h"

using glm::vec3;

namespace Leviathan
{
	Camera::Camera(const float fov, const float near, const float far) :
		m_fovY{ fov }, m_nearPlane{ near }, m_farPlane{ far }, m_theta{ 180.f }, m_phi{ 0.f }, m_location{ 0.f, 1.f, 3.f }
	{}

	mat4 Camera::Projection() const
	{
		int32 w, h;
		glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);

		const mat4 perspective = glm::perspective(
			glm::radians(m_fovY), static_cast<float>(w) / static_cast<float>(h), m_nearPlane, m_farPlane
		);

		const float thetaR = glm::radians(m_theta);
		const float phiR = glm::radians(m_phi);
		const vec3 forward(cos(phiR) * sin(thetaR), sin(phiR), cos(phiR) * cos(thetaR));

		const mat4 view = glm::lookAt(m_location, m_location + forward, vec3{ 0, 1, 0 });

		return perspective * view;
	}
}
