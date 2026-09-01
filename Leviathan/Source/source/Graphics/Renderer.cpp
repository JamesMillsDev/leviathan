#include "Graphics/Renderer.h"

#include <format>
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
	RenderPass::RenderPass() :
		features{ EPassFeature::FaceCulling }, cullFaceMode{ GL_BACK },
		clearMask{ GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT }, frameBuffer{ nullptr },
		passShader{ nullptr }, passMaterial{ nullptr }
	{}

	RenderPass::~RenderPass()
	{
		delete frameBuffer;
		delete passMaterial;
		delete passShader;
	}

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
		m_depthBuffer{ nullptr }, m_screenMesh{ nullptr }, m_gBufferPass{ nullptr }, m_gBuffer{ nullptr }
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

	void Renderer::Render(Material* material, Mesh* mesh, const mat4& transform)
	{
		m_gBufferPass->m_renderFncs.push([this, mesh, transform]
		(const mat4& projection, const mat4& view, const vec3& viewLoc)
			{
				Material* mat = m_gBufferPass->passMaterial;

				if (!mat->Bind())
				{
					return;
				}

				mat->Set(mat->m_vpLoc, projection * view);
				mat->Set(mat->m_cameraLocationLoc, viewLoc);

				mat->Set(mat->m_modelLoc, transform);
				mat->Set(mat->m_normalMatrixLoc, mat3(glm::transpose(glm::inverse(transform))));

				mesh->Render();
			});

		for (RenderPass* pass : m_renderPasses)
		{
			pass->m_renderFncs.push([this, material, pass]
			(const mat4& projection, const mat4& view, const vec3& viewLoc)
				{
					if (!material->Bind())
					{
						return;
					}

					/*if (pass->IsFeatureEnabled(EPassFeature::Lighting))
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
					}*/

					mat4 overrideProjection = projection;
					mat4 overrideView = view;
					vec3 overrideViewLoc = viewLoc;

					/*if (pass->m_lightMatrixGetter != nullptr && pass->IsFeatureEnabled(EPassFeature::WritesShadowMap))
					{
						pass->m_lightMatrixGetter(m_lights[0], overrideProjection, overrideView);
						overrideViewLoc = m_lights[0]->transform[3];
					}*/

					//material->Set(material->m_vpLoc, overrideProjection * overrideView);
					material->Set(material->m_cameraLocationLoc, overrideViewLoc);

					//material->Set(material->m_modelLoc, transform);
					//material->Set(material->m_normalMatrixLoc, mat3(glm::transpose(glm::inverse(transform))));

					material->SetMaterialProperties(m_shadowMap->Handle(), pass->IsFeatureEnabled(EPassFeature::ShadowMapping));

					m_gBuffer->Bind(material);
					material->Set("gBuffer.debugPhase", 2);

					m_screenMesh->Render();
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
			GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER
		};

		m_depthBuffer = new FrameBuffer{
			m_window->m_width, m_window->m_height, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, GL_FLOAT,
			GL_NEAREST, GL_CLAMP_TO_BORDER
		};

		vec3 up = { 0.f, 0.f, 1.f };
		TArray uv = 
		{
			vec2{ 1.f, 0.f },
			vec2{ 0.f, 0.f },
			vec2{ 0.f, 1.f },
			vec2{ 1.f, 1.f },
		};
		m_screenMesh = Mesh::MakePlane(&up, &uv);

		m_gBufferPass = new RenderPass;
		m_gBufferPass->passShader = new Shader{ "Shaders/gbuffer" };
		m_gBufferPass->passMaterial = new Material{ m_gBufferPass->passShader };

		m_gBuffer = new GBuffer{ window->m_width, window->m_height };

		/*RenderPass* fullDepthPass = new RenderPass;
		fullDepthPass->frameBuffer = m_depthBuffer;
		fullDepthPass->clearMask = GL_DEPTH_BUFFER_BIT;

		RenderPass* shadowPass = new RenderPass;
		shadowPass->frameBuffer = m_shadowMap;
		shadowPass->EnablePassFeature(EPassFeature::OverrideCameraMatrices | EPassFeature::WritesShadowMap);
		shadowPass->SetLightMatrixGetter([this](const Light* light, mat4& projection, mat4& view)
			{
				projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 7.5f);
				view = glm::inverse(light->transform);
			});*/

		RenderPass* mainRenderPass = new RenderPass;
		m_renderPasses.Add(mainRenderPass);
	}

	void Renderer::Render()
	{
		// Use the default camera values
		mat4 view = m_camera->View();
		mat4 projection = m_camera->Projection();
		vec3 cameraLoc = view[3];

		// Record to the gBuffer
		m_gBuffer->BeginRecording();
		m_gBufferPass->Render(m_window, projection, view, cameraLoc);
		m_gBuffer->FinishRecording();

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
		for (RenderPass* pass : m_renderPasses)
		{
			delete pass;
		}

		delete m_gBufferPass;
		delete m_screenMesh;
		delete m_gBuffer;

		delete m_depthBuffer;
		delete m_shadowMap;
	}
}
