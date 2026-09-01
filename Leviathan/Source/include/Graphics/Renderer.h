#pragma once

#include <functional>
#include <memory>
#include <queue>

#include <glm/mat4x4.hpp>

using std::queue;
using std::shared_ptr;

using glm::mat4;
using glm::vec3;

namespace Leviathan
{
	class Camera;
	class Config;
	class DepthBuffer;
	class GBuffer;
	class Light;
	class Lighting;
	class Mesh;
	class Material;
	class Window;

	using RenderFnc = std::function<void(const mat4&, const mat4&)>;

	class Renderer
	{
		friend class Application;

	private:
		Camera* m_camera;

		shared_ptr<Window> m_window;
		shared_ptr<Config> m_config;

		Mesh* m_screenMesh;

		DepthBuffer* m_depthBuffer;
		Lighting* m_lighting;
		GBuffer* m_gBuffer;

	private:
		struct PrivateKey
		{};

	public:
		explicit Renderer(PrivateKey);

	public:
		void SetActiveCamera(Camera* camera);
		void AddLight(Light* light) const;
		void RemoveLight(Light* light) const;

		void Render(Material* material, Mesh* mesh, const mat4& transform) const;

	private:
		void Init(const shared_ptr<Window>& window);
		void Render() const;
		void Shutdown() const;

	};
}