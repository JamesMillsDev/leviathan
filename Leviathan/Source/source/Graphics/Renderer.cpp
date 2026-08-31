#include "Graphics/Renderer.h"

#include <glad/gl.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Core/Window.h"
#include "Graphics/Light.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Cameras/Camera.h"
#include "Graphics/Textures/FrameBuffer.h"
#include "Graphics/Textures/GBuffer.h"
#include "Utility/Config.h"

namespace Leviathan
{
	RenderPass_Dep::RenderPass_Dep() :
		RenderPass_Dep{ nullptr, mat4{1.f} }
	{}

	RenderPass_Dep::RenderPass_Dep(const mat4& prjMatrix) :
		RenderPass_Dep{ nullptr, prjMatrix }
	{}

	RenderPass_Dep::RenderPass_Dep(FrameBuffer* fb, const mat4& prjMatrix) :
		viewLocation{ 0.f }, viewMatrix{ 1.f }, projectionMatrix{ prjMatrix }, frameBuffer{ fb },
		useCameraValues{ true }, clearMask{ GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT }, cullFaceMode{ GL_BACK },
		useFaceCulling{ true }, useShadowMapping{ false }, m_cameraTransformGetter{ nullptr }, m_lightMatrixGetter{ nullptr },
		m_preRender{ nullptr }
	{}

	RenderPass_Dep::~RenderPass_Dep() = default;

	void RenderPass_Dep::QueueRender(const function<void()>& fnc)
	{
		m_renderQueue.emplace(fnc);
	}

	void RenderPass_Dep::SetCameraSettingsGetter(const CameraTransformGetter& getter)
	{
		m_cameraTransformGetter = getter;
	}

	void RenderPass_Dep::SetLightMatrixGetter(const LightMatrixGetter& getter)
	{
		m_lightMatrixGetter = getter;
	}

	void RenderPass_Dep::SetPreRenderFnc(const function<void()>& preRenderFnc)
	{
		m_preRender = preRenderFnc;
	}

	void RenderPass_Dep::SetPostRenderFnc(const function<void()>& postRenderFnc)
	{
		m_postRender = postRenderFnc;
	}

	void RenderPass_Dep::Render(const shared_ptr<Window>& window)
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

		if (m_postRender != nullptr)
		{
			m_postRender();
		}
	}

	RenderPass::RenderPass() :
		features{ EPassFeature::FaceCulling }, cullFaceMode{ GL_BACK },
		clearMask{ GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT }, frameBuffer{ nullptr }
	{}

	void RenderPass::SetPreRenderFnc(const VoidSignature& preRender)
	{
		m_preRenderFnc = preRender;
	}

	void RenderPass::SetPostRenderFnc(const VoidSignature& postRender)
	{
		m_postRenderFnc = postRender;
	}

	void RenderPass::SetLightMatrixGetter(const LightMatrixSignature& getter)
	{
		m_lightMatrixGetter = getter;
	}

	void RenderPass::EnablePassFeature(const EPassFeature feature) const
	{
		features |= feature;
	}

	void RenderPass::DisablePassFeature(const EPassFeature feature) const
	{
		features &= ~feature;
	}

	bool RenderPass::IsFeatureEnabled(const EPassFeature feature) const
	{
		return (features & feature) == feature;
	}

	void RenderPass::Render(const shared_ptr<Window>& window, const mat4& projection, const mat4& view,
		const vec3& location)
	{
		// Clear the screen with the correct mask
		window->Clear(clearMask);

		if (m_preRenderFnc != nullptr)
		{
			m_preRenderFnc();
		}

		// Render each item in the queue
		while (!m_renderFncs.empty())
		{
			m_renderFncs.front()(projection, view, location);
			m_renderFncs.pop();
		}

		if (m_postRenderFnc != nullptr)
		{
			m_postRenderFnc();
		}
	}

	Renderer::Renderer(PrivateKey) :
		m_camera{ nullptr }, m_config{ std::make_shared<Config>("Renderer") }, m_shadowMap{ nullptr },
		m_depthBuffer{ nullptr }, m_mainRenderPass{ nullptr }, m_gBuffer{ nullptr }
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

	void Renderer::AddRenderPass(RenderPass_Dep* pass)
	{
		m_depRenderPasses.Add(pass);
	}

	void Renderer::InsertRenderPass(RenderPass_Dep* pass, const int64 index)
	{
		m_depRenderPasses.Insert(pass, index);
	}

	void Renderer::RemoveRenderPass(RenderPass_Dep* pass)
	{
		m_depRenderPasses.Remove(pass);
	}

	void Renderer::Render_Dep(Material* material, Mesh* mesh, const mat4& transform)
	{
		if (m_camera == nullptr)
		{
			return;
		}

		for (RenderPass_Dep* pass : m_depRenderPasses)
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

					material->SetMaterialProperties(m_shadowMap->TextureHandle(), pass->useShadowMapping);

					mesh->Render();
				});
		}
	}

	void Renderer::Render(Material* material, Mesh* mesh, const mat4& transform)
	{
		for (RenderPass* pass : m_renderPasses)
		{
			pass->m_renderFncs.emplace([this, material, mesh, transform, pass]
			(const mat4& projection, const mat4& view, const vec3& viewLoc)
				{
					if (!material->Bind())
					{
						return;
					}

					if (pass->IsFeatureEnabled(EPassFeature::Lighting))
					{
						material->Set("lightCount", static_cast<int32>(m_lights.Count()));

						for (int32 i = 0; i < static_cast<int32>(m_lights.Count()); ++i)
						{
							m_lights[i]->SetMaterialProperties(material, i);

							if (pass->m_lightMatrixGetter != nullptr)
							{
								mat4 lightProjection;
								mat4 lightView;
								pass->m_lightMatrixGetter(m_lights[i], lightProjection, lightView);

								material->Set(std::format("lightSpaceMatrices[{}]", i), lightProjection * lightView);
							}
						}
					}

					mat4 overrideProjection = projection;
					mat4 overrideView = view;
					vec3 overrideViewLoc = viewLoc;

					if (pass->m_lightMatrixGetter != nullptr && pass->IsFeatureEnabled(EPassFeature::WritesShadowMap))
					{
						pass->m_lightMatrixGetter(m_lights[0], overrideProjection, overrideView);
						overrideViewLoc = m_lights[0]->transform[3];
					}

					material->Set(material->m_vpLoc, overrideProjection * overrideView);
					material->Set(material->m_cameraLocationLoc, overrideViewLoc);

					material->Set(material->m_modelLoc, transform);
					material->Set(material->m_normalMatrixLoc, mat3(glm::transpose(glm::inverse(transform))));

					material->SetMaterialProperties(m_shadowMap->Handle(), pass->IsFeatureEnabled(EPassFeature::ShadowMapping));

					mesh->Render();
				});
		}
	}

	FrameBuffer* Renderer::GetShadowMap() const
	{
		return m_shadowMap;
	}

	RenderPass_Dep* Renderer::GetMainRenderPass() const
	{
		return m_mainRenderPass;
	}

	void Renderer::Init(const shared_ptr<Window>& window)
	{
		m_window = window;

		m_shadowMap = new FrameBuffer{
			m_config->Get<int32>("Shadows.Map.Width"), m_config->Get<int32>("Shadows.Map.Height"),
			GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER
		};

		m_depthBuffer = new FrameBuffer{
			m_window->m_width, m_window->m_height, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, GL_FLOAT,
			GL_NEAREST, GL_CLAMP_TO_BORDER
		};

		/*RenderPass_Dep* fullDepthPass = new RenderPass_Dep{ m_depthBuffer, mat4{1.f} };
		fullDepthPass->clearMask = GL_DEPTH_BUFFER_BIT;

		RenderPass_Dep* shadowPass = new RenderPass_Dep{ m_shadowMap, mat4{1.f} };
		shadowPass->useCameraValues = false;
		shadowPass->SetLightMatrixGetter([this](const Light* light, mat4& projection, mat4& view)
			{
				projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 7.5f);
				view = glm::inverse(light->transform);
			});*/

		m_gBuffer = new GBuffer{ window->m_width, window->m_height };

		RenderPass* gPass = new RenderPass;
		gPass->EnablePassFeature(EPassFeature::WritesGBuffer);

		RenderPass* fullDepthPass = new RenderPass;
		fullDepthPass->frameBuffer = m_depthBuffer;
		fullDepthPass->clearMask = GL_DEPTH_BUFFER_BIT;

		RenderPass* shadowPass = new RenderPass;
		shadowPass->frameBuffer = m_shadowMap;
		shadowPass->EnablePassFeature(EPassFeature::OverrideCameraMatrices | EPassFeature::WritesShadowMap);
		shadowPass->SetLightMatrixGetter([this](const Light* light, mat4& projection, mat4& view)
			{
				projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 7.5f);
				view = glm::inverse(light->transform);
			});

		m_renderPasses.Add(fullDepthPass);
		m_renderPasses.Add(shadowPass);
		m_renderPasses.Add(gPass);

		//m_mainRenderPass = new RenderPass_Dep;
		//m_mainRenderPass->useShadowMapping = true;

		//AddRenderPass(fullDepthPass);
		//AddRenderPass(shadowPass);
		//AddRenderPass(testGPass);
		//AddRenderPass(m_mainRenderPass);
	}

	void Renderer::Render_Dep()
	{
		for (RenderPass_Dep* pass : m_depRenderPasses)
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

	void Renderer::Render()
	{
		vec3 cameraLoc;
		mat4 projection, view;

		for (RenderPass* pass : m_renderPasses)
		{
			// Get the current state of the face culling
			int32 currentCullMode;
			glGetIntegerv(GL_CULL_FACE_MODE, &currentCullMode);

			// If there is a mismatch in the face culling enabled state, change it
			if (!pass->IsFeatureEnabled(EPassFeature::FaceCulling))
			{
				glDisable(GL_CULL_FACE);
			}

			// If there is a mismatch in the face culling mode, change it
			if (currentCullMode != pass->cullFaceMode)
			{
				glCullFace(pass->cullFaceMode);
			}

			// If we are overriding the camera matrices, run the getter
			if (pass->IsFeatureEnabled(EPassFeature::OverrideCameraMatrices))
			{
				pass->m_cameraMatrixGetter(projection, view, cameraLoc);
			}
			else
			{
				// Use the default camera values
				view = m_camera->View();
				projection = m_camera->Projection();
				cameraLoc = view[3];
			}

			// If this pass has a frame buffer, bind it and set the viewport
			if (pass->frameBuffer != nullptr)
			{
				pass->frameBuffer->Bind();
				glViewport(0, 0, pass->frameBuffer->Width(), pass->frameBuffer->Height());
			}

			pass->Render(m_window, projection, view, cameraLoc);

			// If there was a frame buffer bound, unbind and reset the viewport to the screen.
			if (pass->frameBuffer != nullptr)
			{
				pass->frameBuffer->Unbind();
				glViewport(0, 0, m_window->m_width, m_window->m_height);
			}

			// Reenable face culling 
			glEnable(GL_CULL_FACE);
			if (currentCullMode != pass->cullFaceMode)
			{
				glCullFace(currentCullMode);
			}
		}
	}

	void Renderer::Shutdown() const
	{
		for (RenderPass_Dep* pass : m_depRenderPasses)
		{
			delete pass;
		}

		for (RenderPass* pass : m_renderPasses)
		{
			delete pass;
		}

		delete m_gBuffer;
		delete m_depthBuffer;
		delete m_shadowMap;
	}
}
