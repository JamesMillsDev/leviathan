#pragma once

#include <glm/mat4x4.hpp>

using glm::mat4;

namespace Leviathan
{
	class Camera;
	class Mesh;
	class Material;

	class Renderer
	{
		friend class Application;

	private:
		Camera* m_camera;

	private:
		struct PrivateKey
		{};

	public:
		explicit Renderer(PrivateKey);

	public:
		void SetActiveCamera(Camera* camera);
		void Render(Material* material, Mesh* mesh, const mat4& transform) const;

	};
}