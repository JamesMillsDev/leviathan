#include "Graphics/RenderPasses/IRenderPass.h"

#include <glad/gl.h>

#include "Graphics/Resources/Material.h"
#include "Graphics/Resources/Shader.h"

namespace Leviathan
{
	IRenderPass::IRenderPass(const std::string& shader)
		: m_handle{ 0 }, m_bound{ false }, m_passShader{ new Shader{ "Shaders/" + shader } }
	{
		m_passMaterial = new Material{ m_passShader };
	}

	IRenderPass::~IRenderPass()
	{
		delete m_passMaterial;
		delete m_passShader;

		glDeleteFramebuffers(1, &m_handle);
	}

	void IRenderPass::BeginRecording()
	{
		// Validate the handle isn't 0
		if (m_handle == 0)
		{
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
		m_bound = true;
	}

	void IRenderPass::FinishRecording()
	{
		if (!m_bound)
		{
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_bound = false;
	}

	void IRenderPass::QueueRender(Material* material, Mesh* mesh, const glm::mat4& transform)
	{
		m_renderQueue.push(
			{
				.material = material,
				.mesh = mesh,
				.transform = transform,
				.fnc = [this]<typename T0, typename T1, typename T2, typename T3>(T0 && ph1, T1 && ph2, T2 && ph3, T3 && ph4)
				{
					RenderFnc(std::forward<T0>(ph1), std::forward<T1>(ph2),
							  std::forward<T2>(ph3), std::forward<T3>(ph4));
				}
			}
		);
	}

	void IRenderPass::Render(const mat4& projection, const mat4& view, const vec3& viewLoc)
	{
		BeginRecording();
		glClear(ClearFlags());

		if (!m_passMaterial->Bind())
		{
			FinishRecording();
			return;
		}

		while (!m_renderQueue.empty())
		{
			PassRenderInfo render = m_renderQueue.front();
			render.fnc(projection, view, viewLoc, render);

			m_renderQueue.pop();
		}

		FinishRecording();
	}

	uint32 IRenderPass::Handle() const
	{
		return m_handle;
	}
}
