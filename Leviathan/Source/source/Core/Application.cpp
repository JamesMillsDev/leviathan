#include "Core/Application.h"

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>

#include "Core/GameTime.h"
#include "Core/Input.h"
#include "Core/Window.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Cameras/OrbitCamera.h"
#include "Utility/Config.h"

namespace Leviathan
{
	Application::Application()
		: m_engineConfig{ std::make_shared<Config>("Engine") },
		m_window{ std::make_shared<Window>(m_engineConfig, Window::PrivateKey{}) },
		m_renderer{ std::make_shared<Renderer>(Renderer::PrivateKey{}) }
	{

	}

	weak_ptr<Window> Application::GetWindow() const
	{
		return m_window;
	}

	weak_ptr<GameInstance> Application::GetGameInstance() const
	{
		return m_game;
	}

	int32 Application::Run() const
	{
		// Attempt to open the window
		if (m_window->Open())
		{
			Input::Create();
			GameTime::Init();

			Shader* shader = new Shader{ "Shaders/lit" };
			Material* material = new Material{ shader };
			material->SetMaterialProperty("material.tint", EMaterialPropertyType::Vec3, { .v3Value = vec3{ 1.f, .5f, .31f } });
			material->SetMaterialProperty("material.specularStrength", EMaterialPropertyType::Float, { .fValue = 1.f });
			Mesh* mesh = Mesh::MakeFromAssimp("Meshes/shaderBall.fbx"); 

			Shader* cubeShader = new Shader{ "Shaders/unlit" };
			Material* cubeMaterial = new Material{ cubeShader };
			cubeMaterial->SetMaterialProperty("material.tint", EMaterialPropertyType::Vec3, { .v3Value = vec3{ 1.f } });
			Mesh* cubeMesh = Mesh::MakeCube();

			OrbitCamera* camera = new OrbitCamera;

			mat4 model = glm::scale(mat4(1.f), vec3{ .5f });
			mat4 cubeModel = glm::translate(mat4(1.f), vec3{ 1.2f, 1.f, 2.f });
			cubeModel = glm::scale(cubeModel, vec3{ .2f });

			m_renderer->m_camera = camera;

			Input* input = Input::GetInstance();

			// The window opened successfully, so run the render loop
			while (m_window->IsOpen())
			{
				GameTime::Tick();

				// Will return false if the application is Iconified
				if (!m_window->NewFrame())
				{
					continue;
				}

				if (input->IsMouseButtonDown(EInputCodes::MouseButtonRight))
				{
					vec2 mouseDelta{};
					input->GetMouseDelta(&mouseDelta.x, &mouseDelta.y);

					camera->Rotate(mouseDelta); 
				}

				m_renderer->Render(material, mesh, model);
				m_renderer->Render(cubeMaterial, cubeMesh, cubeModel);

				// Present the current window
				m_window->Present();

				input->ClearStatus();
			}

			delete cubeMaterial;
			delete cubeShader;
			delete cubeMesh; 

			delete camera;
			delete mesh;
			delete material;
			delete shader;

			// Close the window safely
			m_window->Close();

			return EXIT_SUCCESS;
		}

		return EXIT_FAILURE;
	}
}
