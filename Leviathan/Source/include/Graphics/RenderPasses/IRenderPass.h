#pragma once

#include <functional>
#include <queue>
#include <string>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "Maths/Alias.h"
#include "Utility/Collections/TMap.h"

namespace Leviathan
{
	class Material;
	class Mesh;
	class Shader;

	using PassRenderFunc = std::function<void(const glm::mat4&, const glm::mat4&, const glm::vec3&, const struct PassRenderInfo&)>;
	struct PassRenderInfo
	{
		Material* material;
		Mesh* mesh;
		glm::mat4 transform;
		PassRenderFunc fnc;
	};

	class IRenderPass
	{
		friend class Renderer;

	protected:
		Material* m_passMaterial;

	private:
		uint32 m_handle;
		bool m_bound;

		Shader* m_passShader;
		std::queue<PassRenderInfo> m_renderQueue;

	protected:
		explicit IRenderPass(const std::string& shader);
		~IRenderPass();

	protected:
		virtual void RenderFnc(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& viewLoc, const PassRenderInfo& info) = 0;
		virtual uint32 ClearFlags() = 0;

	private:
		void BeginRecording();
		void FinishRecording();

		void QueueRender(Material* material, Mesh* mesh, const glm::mat4& transform);
		void Render(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& viewLoc);

		[[nodiscard]] uint32 Handle() const;

	};
}