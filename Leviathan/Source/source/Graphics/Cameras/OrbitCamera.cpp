#include "Graphics/Cameras/OrbitCamera.h"

#include <glm/ext/matrix_transform.hpp>

namespace Leviathan
{
	OrbitCamera::OrbitCamera(const float fov, const float near, const float far)
		: Camera{ fov, near, far }, m_target{ 0.f, 1.f, 0.f }, m_distance{ 2.f }, m_yaw{ -90.f },
		m_pitch{ 0.f }, m_minDistance{ 1.f }, m_maxDistance{ 50.f }, m_minPitch{ -89.f },
		m_maxPitch{ 89.f }
	{

	}

	void OrbitCamera::SetMinDistance(const float min)
	{
		m_minDistance = glm::clamp(min, 1.f, m_maxDistance - 1.f);
	}

	void OrbitCamera::SetMaxDistance(const float max)
	{
		m_maxDistance = glm::clamp(max, m_minDistance + 1.f, 1000.f);
	}

	void OrbitCamera::SetMinPitch(const float min)
	{
		m_maxPitch = glm::clamp(min, -89.f, 0.f);
	}

	void OrbitCamera::SetMaxPitch(const float max)
	{
		m_maxPitch = glm::clamp(max, 0.f, 89.f);
	}

	void OrbitCamera::SetTarget(const vec3& target)
	{
		m_target = target;
	}

	void OrbitCamera::Rotate(const vec2 delta, const float sensitivity)
	{
		m_yaw += delta.x * sensitivity;
		m_pitch -= delta.y * sensitivity;

		m_pitch = glm::clamp(m_pitch, m_minPitch, m_maxPitch);
	}

	void OrbitCamera::Zoom(const float delta, const float sensitivity)
	{
		m_distance -= delta * sensitivity;
		m_distance = glm::clamp(m_distance, m_minDistance, m_maxDistance);
	}

	mat4 OrbitCamera::View() const
	{
		const vec3 location = Location();
		constexpr vec3 up = vec3{ 0.f, 1.f, 0.f };

		return glm::lookAt(location, m_target, up);
	}

	vec3 OrbitCamera::Location() const
	{
		const float rYaw = glm::radians(m_yaw);
		const float rPitch = glm::radians(m_pitch);

		const float x = m_distance * cos(rPitch) * cos(rYaw);
		const float y = m_distance * sin(rPitch);
		const float z = m_distance * cos(rPitch) * sin(rYaw);

		return m_target + vec3{ x, y, z };
	}
}
