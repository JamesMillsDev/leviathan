#include "Graphics/Renderer.h"

#include <glad/gl.h>

#include "Core/Window.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "Graphics/Camera.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Light.h"
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
		useFaceCulling{ true }, m_cameraTransformGetter{ nullptr }, m_lightMatrixGetter{ nullptr }, m_preRender{ nullptr }
	{}

	RenderPass::~RenderPass() = default;

	void RenderPass::QueueRender(const function<void()>& fnc)
	{
		m_renderQueue.emplace(fnc);
	}

	void RenderPass::SetCameraSettingsGetter(const CameraTransformGetter& getter)
	{
		m_cameraTransformGetter = getter;
	}

	void RenderPass::SetLightMatrixGetter(const LightMatrixGetter& getter)
	{
		m_lightMatrixGetter = getter;
	}

	void RenderPass::SetPreRenderFnc(const function<void()>& preRenderFnc)
	{
		m_preRender = preRenderFnc;
	}

	void RenderPass::Render(const shared_ptr<Window>& window)
	{
		// Clear the screen with the correct mask
		window->Clear(clearMask);

		if (m_preRender != nullptr)
		{
			m_preRender();
		}

		// Render each item in the queue
		while (!m_renderQueue.empty())
		{
			m_renderQueue.front()();
			m_renderQueue.pop();
		}
	}

	Renderer::Renderer(PrivateKey) :
		m_camera{ nullptr }, m_config{ std::make_shared<Config>("Renderer") }, m_shadowMap{ nullptr },
		m_depthBuffer{ nullptr }, m_mainRenderPass{ nullptr }
	{}

	void Renderer::SetActiveCamera(Camera* camera)
	{
		m_camera = camera;
	}

	void Renderer::AddLight(Light* light)
	{
		m_lights.Add(light);
	}

	void Renderer::RemoveLight(Light* light)
	{
		m_lights.Remove(light);
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

					material->Set("lightCount", static_cast<int32>(m_lights.Count()));
					for (int32 i = 0; i < static_cast<int32>(m_lights.Count()); ++i)
					{
						m_lights[i]->SetMaterialProperties(material, i);

						if (pass->m_lightMatrixGetter != nullptr)
						{
							mat4 projection;
							mat4 view;
							pass->m_lightMatrixGetter(m_lights[i], projection, view);

							material->Set(std::format("lightSpaceMatrices[{}]", i), projection * view);
						}
					}

					if (pass->m_lightMatrixGetter != nullptr && !pass->useCameraValues)
					{
						pass->m_lightMatrixGetter(m_lights[0], pass->projectionMatrix, pass->viewMatrix);
						pass->viewLocation = m_lights[0]->transform[3];
					}

					material->Set(material->m_vpLoc, pass->projectionMatrix * pass->viewMatrix);
					material->Set(material->m_cameraLocationLoc, pass->viewLocation);

					material->Set(material->m_modelLoc, transform);
					material->Set(material->m_normalMatrixLoc, mat3(glm::transpose(glm::inverse(transform))));

					material->SetMaterialProperties(m_shadowMap->TextureHandle());

					mesh->Render();
				});
		}
	}

	FrameBuffer* Renderer::GetShadowMap() const
	{
		return m_shadowMap;
	}

	RenderPass* Renderer::GetMainRenderPass() const
	{
		return m_mainRenderPass;
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
			GL_NEAREST, GL_CLAMP_TO_BORDER
		};

		RenderPass* fullDepthPass = new RenderPass{ m_depthBuffer, mat4{ 1.f } };
		fullDepthPass->clearMask = GL_DEPTH_BUFFER_BIT;

		RenderPass* shadowPass = new RenderPass{ m_shadowMap, mat4{ 1.f } };
		shadowPass->useCameraValues = false;
		shadowPass->SetLightMatrixGetter([this](const Light* light, mat4& projection, mat4& view)
			{
				projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 7.5f);
				view = light->transform;
			});

		m_mainRenderPass = new RenderPass;

		AddRenderPass(fullDepthPass);
		AddRenderPass(shadowPass);
		AddRenderPass(m_mainRenderPass);
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
			else if (pass->m_cameraTransformGetter != nullptr)
			{
				pass->m_cameraTransformGetter(pass->projectionMatrix, pass->viewMatrix, pass->viewLocation);
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
