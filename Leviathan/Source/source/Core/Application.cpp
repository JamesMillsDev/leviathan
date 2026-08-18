#include "Core/Application.h"

#include "Core/Window.h"

#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"

#include "Utility/Config.h"

namespace Leviathan
{
	Application::Application()
		: m_engineConfig{ std::make_shared<Config>("Engine") },
		  m_window{ std::make_shared<Window>(m_engineConfig, Window::PrivateKey{}) }
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
			Shader* shader = new Shader{ "Shaders/unlit" };
			Material* material = new Material{ shader };

			Mesh* mesh = Mesh::MakeFromAssimp("Meshes/shaderBall.fbx");

			// The window opened successfully, so run the render loop
			while (m_window->IsOpen())
			{
				// Will return false if the application is Iconified
				if (!m_window->NewFrame())
				{
					continue;
				}

				if (material->Bind())
				{
					mesh->Render();
				}

				// Present the current window
				m_window->Present();
			}

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