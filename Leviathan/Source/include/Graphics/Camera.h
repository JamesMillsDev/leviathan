#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

using glm::mat4;
using glm::vec3;

namespace Leviathan
{
	class Camera
	{
	private:
		float m_fovY;
		float m_nearPlane;
		float m_farPlane;

		float m_theta;
		float m_phi;
		vec3 m_location;

	public:
		explicit Camera(float fov = 90.f, float near = .1f, float far = 1000.f);

	public:
		mat4 Projection() const;

	};
}
