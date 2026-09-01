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
	class FrameBuffer;
	class Mesh;
	class Material;
	class Shader;
	class Window;

	using RenderFnc = std::function<void(const mat4&, const mat4&)>;

	class DepthBuffer
	{
		friend class Renderer;

	private:
		shared_ptr<Window> m_window;

		queue<RenderFnc> m_renders;
		bool m_bound;

		FrameBuffer* m_frameBuffer;

		Shader* m_shader;
		Material* m_material;

	private:
		explicit DepthBuffer(const shared_ptr<Window>& window);
		~DepthBuffer();

	private:
		void BeginRecording();
		void QueueRender(Mesh* mesh, const mat4& transform);
		void Render(const mat4& projection, const mat4& view);
		void FinishRecording();

	};
}