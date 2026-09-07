#include "Graphics/Renderer.h"

#include <format>
#include <glad/gl.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Core/Window.h"

#include "Graphics/DepthBuffer.h"
#include "Graphics/GBuffer.h"
#include "Graphics/Lighting.h"
#include "Graphics/Shadows.h"
#include "Graphics/Cameras/Camera.h"
#include "Graphics/Resources/Material.h"
#include "Graphics/Resources/Mesh.h"
#include "Graphics/Resources/Textures/FrameBuffer.h"
#include "Utility/Config.h"

namespace Leviathan
{
	Renderer::Renderer(PrivateKey) :
		m_camera{ nullptr }, m_config{ std::make_shared<Config>("Renderer") }, m_screenMesh{ nullptr },
		m_depthBuffer{ nullptr }, m_lighting{ nullptr }, m_gBuffer{ nullptr }, m_shadows{ nullptr }, m_finalRender{ nullptr },
		m_finalRenderShader{ nullptr }, m_finalRenderMaterial{ nullptr }, m_unlitRenderMaterial{ nullptr }
	{}

	void Renderer::SetActiveCamera(Camera* camera)
	{
		m_camera = camera;
	}

	void Renderer::AddLight(Light* light) const
	{
		m_lighting->m_lights.Add(light);
	}

	void Renderer::RemoveLight(Light* light) const
	{
		m_lighting->m_lights.Remove(light);
	}

	void Renderer::Render(Material* material, Mesh* mesh, const mat4& transform) const
	{
		m_depthBuffer->QueueRender(mesh, transform);
		m_gBuffer->QueueRender(material, mesh, transform);
		m_shadows->QueueRender(mesh, transform);
	}

	void Renderer::Init(const shared_ptr<Window>& window)
	{
		m_window = window;

		vec3 up = { 0.f, 0.f, 1.f };
		TArray uv =
		{
			vec2{ 1.f, 0.f },
			vec2{ 0.f, 0.f },
			vec2{ 0.f, 1.f },
			vec2{ 1.f, 1.f },
		};
		m_screenMesh = Mesh::MakePlane(&up, &uv);

		m_depthBuffer = new DepthBuffer{ window };
		m_lighting = new Lighting{ m_config };
		m_gBuffer = new GBuffer{ window };
		m_shadows = new Shadows{ window, m_config };

		m_finalRender = new FrameBuffer{
			m_window->Width(), m_window->Height(), GL_RGBA, GL_COLOR_ATTACHMENT0, GL_UNSIGNED_BYTE,
			GL_NEAREST, GL_CLAMP_TO_EDGE
		};
		 
		m_finalRenderShader = new Shader{
			"Shaders/output"
		};
		m_finalRenderMaterial = new Material{ m_finalRenderShader };
	}

	void Renderer::Render() const
	{
		// Use the default camera values
		const mat4 view = m_camera->View();
		const mat4 projection = m_camera->Projection();
		const vec3 cameraLoc = view[3];

		// Render the frame
		m_depthBuffer->Render(projection, view);
		m_shadows->Render(m_lighting->m_lights);
		m_gBuffer->Render(projection, view, cameraLoc);

		m_window->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		CopyBuffer(GL_DEPTH_BUFFER_BIT, GL_NEAREST, m_finalRender->Handle());

		// Render the lighting onto the lit objects.
		m_lighting->Render(m_screenMesh, m_gBuffer, cameraLoc, m_shadows->m_shadowMap->TextureHandle());

		// Generate the buffer
		m_window->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_gBuffer->Render(projection, view, cameraLoc);
		CopyBuffer(GL_COLOR_BUFFER_BIT, GL_NEAREST, m_finalRender->Handle());
		//CopyBuffer(GL_DEPTH_BUFFER_BIT, GL_NEAREST, m_finalRender->Handle());

		// Render the final output
		if (!m_finalRenderMaterial->Bind())
		{
			return;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_finalRender->TextureHandle());
		m_finalRenderMaterial->Set("finalColor", static_cast<int32>(m_finalRender->TextureHandle()));

		m_screenMesh->Render();
	}

	void Renderer::Shutdown() const
	{
		delete m_finalRenderMaterial;
		delete m_finalRenderShader;
		delete m_finalRender;

		delete m_screenMesh;
		delete m_shadows;
		delete m_depthBuffer;
		delete m_lighting;
		delete m_gBuffer;
	}

	void Renderer::CopyBuffer(const uint32 buffer, const uint32 filter, uint32 target) const
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gBuffer->Handle());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target); // write to target framebuffer
		glBlitFramebuffer(
			0, 0, m_window->m_width, m_window->m_height, 0, 0, m_window->m_width,
			m_window->m_height, buffer, filter
		);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
