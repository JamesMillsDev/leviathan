#pragma once

#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "Utility/Collections/TList.h"
#include "Utility/Collections/TMap.h"

using std::queue;
using std::shared_ptr;
using std::string;

using glm::mat4;
using glm::vec3;

namespace Leviathan
{
	using RenderFunc = std::function<void(const mat4&, const mat4&, const vec3&, const struct GBufferRender&)>;

	class Material;
	class Mesh;
	class Shader;
	class Window;

	struct GBufferRender
	{
		Material* material;
		Mesh* mesh;
		mat4 transform;
		RenderFunc fnc;
	};

	class GBuffer
	{
		friend class Lighting;
		friend class Renderer;

	private:
		struct GBufferHandle
		{
			uint32 handle;
			bool render = true;
		};

	private:
		static GBufferHandle GenerateTextureBuffer(int32 w, int32 h, int32 format, uint32 type, TList<uint32>& attachments);
		static GBufferHandle GenerateDepthAttachment(int32 w, int32 h);

	private:
		uint32 m_handle;
		bool m_bound;

		TMap<string, GBufferHandle> m_bufferHandles;
		queue<GBufferRender> m_render;

		Material* m_material;
		Shader* m_shader;

	private:
		explicit GBuffer(const shared_ptr<Window>& window);
		~GBuffer();

	private:
		void BeginRecording();
		void FinishRecording();

		void Render(const mat4& projection, const mat4& view, const vec3& viewLoc);
		void QueueRender(Material* material, Mesh* mesh, const mat4& transform);

		void Bind(Material* material);
		[[nodiscard]] uint32 Handle() const;

	};
}
