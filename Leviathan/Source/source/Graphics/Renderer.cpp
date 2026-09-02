#include "Graphics/Renderer.h"

#include <format>
#include <glad/gl.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Core/Window.h"

#include "Graphics/DepthBuffer.h"
#include "Graphics/GBuffer.h"
#include "Graphics/Lighting.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Cameras/Camera.h"
#include "Utility/Config.h"

namespace Leviathan
{
	Renderer::Renderer(PrivateKey) :
		m_camera{ nullptr }, m_config{ std::make_shared<Config>("Renderer") }, m_screenMesh{ nullptr },
		m_depthBuffer{ nullptr }, m_lighting{ nullptr }, m_gBuffer{ nullptr }
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
		m_gBuffer->QueueForRender(material, mesh, transform);
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
	}

	void Renderer::Render() const
	{
		// Use the default camera values
		const mat4 view = m_camera->View();
		const mat4 projection = m_camera->Projection();
		const vec3 cameraLoc = view[3];

		// Render the frame
		m_depthBuffer->Render(projection, view);
		m_gBuffer->Render(projection, view, cameraLoc);
		m_window->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_lighting->Render(m_screenMesh, m_gBuffer, cameraLoc);
	}

	void Renderer::Shutdown() const
	{
		delete m_screenMesh;
		delete m_depthBuffer;
		delete m_lighting;
		delete m_gBuffer;
	}
}
