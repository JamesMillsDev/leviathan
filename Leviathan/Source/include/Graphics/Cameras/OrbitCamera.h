#pragma once

#include "Graphics/Cameras/Camera.h"

using glm::vec2;

namespace Leviathan
{
	class OrbitCamera : public Camera
	{
	private:
		vec3 m_target;

		float m_distance;
		float m_yaw;
		float m_pitch;

		float m_minDistance;
		float m_maxDistance;

		float m_minPitch;
		float m_maxPitch;

	public:
		explicit OrbitCamera(float fov = 90.f, float near = .1f, float far = 1000.f);

	public:
		void SetMinDistance(float min);
		void SetMaxDistance(float max);

		void SetMinPitch(float min);
		void SetMaxPitch(float max);

		void SetTarget(const vec3& target);

		void Rotate(vec2 delta, float sensitivity = .1f);
		void Zoom(float delta, float sensitivity = .5f);

		mat4 View() const override;
		vec3 Location() const override;
	};
}
