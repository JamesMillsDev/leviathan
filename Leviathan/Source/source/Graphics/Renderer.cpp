#include "Graphics/Renderer.h"

#include <glad/gl.h>

#include "Core/Window.h"
#include "Graphics/Camera.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Utility/Config.h"

namespace Leviathan
{
	RenderPass::RenderPass()
		: RenderPass{ nullptr, mat4{ 1.f } }
	{}

	RenderPass::RenderPass(const mat4& prjMatrix)
		: RenderPass{ nullptr, prjMatrix }
	{

	}

	RenderPass::RenderPass(FrameBuffer* fb, const mat4& prjMatrix) :
		viewLocation{ 0.f }, viewMatrix{ 1.f }, projectionMatrix{ prjMatrix }, frameBuffer{ fb },
		useCameraValues{ true }, clearMask{ GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT }, cullFaceMode{ GL_BACK },
		useFaceCulling{ true }, m_cameraSettingsGetter{ nullptr }
	{}

	RenderPass::~RenderPass() = default;

	void RenderPass::QueueRender(const function<void()>& fnc)
	{
		m_renderQueue.emplace(fnc);
	}

	void RenderPass::SetCameraSettingsGetter(const function<void(mat4&, mat4&, vec3&)>& getter)
	{
		m_cameraSettingsGetter = getter;
	}

	void RenderPass::Render(const shared_ptr<Window>& window)
	{
		// Clear the screen with the correct mask
		window->Clear(clearMask);

		// Render each item in the queue
		while (!m_renderQueue.empty())
		{
			m_renderQueue.front()();
			m_renderQueue.pop();
		}
	}

	Renderer::Renderer(PrivateKey)
		: m_camera{ nullptr }, m_config{ std::make_shared<Config>("Renderer") }, m_shadowMap{ nullptr },
		m_depthBuffer{ nullptr }
	{

	}

	void Renderer::SetActiveCamera(Camera* camera)
	{
		m_camera = camera;
	}

	void Renderer::AddRenderPass(RenderPass* pass)
	{
		m_renderPasses.Add(pass);
	}

	void Renderer::InsertRenderPass(RenderPass* pass, const int64 index)
	{
		m_renderPasses.Insert(pass, index);
	}

	void Renderer::RemoveRenderPass(RenderPass* pass)
	{
		m_renderPasses.Remove(pass);
	}

	void Renderer::Render(Material* material, Mesh* mesh, const mat4& transform)
	{
		if (m_camera == nullptr)
		{
			return;
		}

		for (RenderPass* pass : m_renderPasses)
		{
			pass->m_renderQueue.push([this, material, mesh, transform, pass]
				{
					if (!material->Bind())
					{
						return;
					}

					material->Set(material->m_vpLoc, pass->projectionMatrix * pass->viewMatrix);
					material->Set(material->m_cameraLocationLoc, pass->viewLocation);

					material->Set(material->m_modelLoc, transform);
					material->Set(material->m_normalMatrixLoc, mat3(glm::transpose(glm::inverse(transform))));

					material->SetMaterialProperties();
					material->Set("lights[0].color", vec3{ 1.f });
					material->Set("lights[0].location", vec3{ 1.2f, 1.0f, 2.0f });

					mesh->Render();
				});
		}
	}

	FrameBuffer* Renderer::GetShadowMap() const
	{
		return m_shadowMap;
	}

	void Renderer::Init(const shared_ptr<Window>& window)
	{
		m_window = window;

		m_shadowMap = new FrameBuffer{
			m_config->Get<int32>("Shadows.Map.Width"), m_config->Get<int32>("Shadows.Map.Height"),
			GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, GL_FLOAT, GL_NEAREST, GL_REPEAT
		};

		m_depthBuffer = new FrameBuffer{
			m_window->m_width, m_window->m_height, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, GL_FLOAT,
			GL_NEAREST, GL_REPEAT
		};

		RenderPass* fullDepthPass = new RenderPass{ m_depthBuffer, mat4{ 1.f } };
		fullDepthPass->clearMask = GL_DEPTH_BUFFER_BIT;

		AddRenderPass(fullDepthPass);
		AddRenderPass(new RenderPass); // Regular render pass
	}

	void Renderer::Render()
	{
		for (RenderPass* pass : m_renderPasses)
		{
			// Get the current state of the face culling
			int32 currentCullMode;
			glGetIntegerv(GL_CULL_FACE_MODE, &currentCullMode);
			const bool faceCullingEnabled = glIsEnabled(GL_CULL_FACE);

			// If there is a mismatch in the face culling enabled state, change it
			if (faceCullingEnabled != pass->useFaceCulling)
			{
				glDisable(GL_CULL_FACE);
			}

			// If there is a mismatch in the face culling mode, change it
			if (currentCullMode != pass->cullFaceMode)
			{
				glCullFace(pass->cullFaceMode);
			}

			// Cache the camera values if requested
			if (pass->useCameraValues)
			{
				pass->viewMatrix = m_camera->View();
				pass->projectionMatrix = m_camera->Projection();
				pass->viewLocation = m_camera->Location();
			}
			else if (pass->m_cameraSettingsGetter != nullptr)
			{
				pass->m_cameraSettingsGetter(pass->projectionMatrix, pass->viewMatrix, pass->viewLocation);
			}

			// If this pass has a frame buffer, bind it and set the viewport
			if (pass->frameBuffer != nullptr)
			{
				pass->frameBuffer->Bind();
				glViewport(0, 0, pass->frameBuffer->Width(), pass->frameBuffer->Height());
			}

			pass->Render(m_window);

			// If there was a frame buffer bound, unbind and reset the viewport to the screen.
			if (pass->frameBuffer != nullptr)
			{
				pass->frameBuffer->Unbind();
				glViewport(0, 0, m_window->m_width, m_window->m_height);
			}

			// If either the face culling mode or face culling enabled changed, reset them to the original
			if (faceCullingEnabled != pass->useFaceCulling)
			{
				glEnable(GL_CULL_FACE);
			}

			if (currentCullMode != pass->cullFaceMode)
			{
				glCullFace(currentCullMode);
			}
		}
	}

	void Renderer::Shutdown() const
	{
		for (RenderPass* pass : m_renderPasses)
		{
			delete pass;
		}

		delete m_depthBuffer;
		delete m_shadowMap;
	}
}
