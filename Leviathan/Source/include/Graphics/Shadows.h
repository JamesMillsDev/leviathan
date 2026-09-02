#pragma once

#include <functional>
#include <memory>
#include <queue>

#include <glm/mat4x4.hpp>

#include "Utility/Collections/TArray.h"
#include "Utility/Collections/TList.h"

using std::queue;
using std::shared_ptr;

using glm::mat4;
using glm::vec3;

namespace Leviathan
{
	class Config;
	class FrameBuffer;
	class Light;
	class Mesh;
	class Material;
	class Shader;
	class Window;

	using RenderFnc = std::function<void(const mat4&, const mat4&)>;

	class Shadows
	{
		friend class Renderer;

	private:
		shared_ptr<Window> m_window;

		queue<RenderFnc> m_renders;
		bool m_bound;

		FrameBuffer* m_shadowMap;

		Shader* m_shader;
		Material* m_material;

	private:
		explicit Shadows(const shared_ptr<Window>& window, const shared_ptr<Config>& config);
		~Shadows();

	private:
		void BeginRecording();
		void QueueRender(Mesh* mesh, const mat4& transform);
		void Render(const TList<Light*>& lights);
		void FinishRecording();

	};
}