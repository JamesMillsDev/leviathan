#include "Core/Application.h"

#include "Core/GameTime.h"
#include "Core/Input.h"
#include "Core/Window.h"
#include "Gameplay/GameInstance.h"
#include "Graphics/Renderer.h"
#include "Utility/Config.h"

namespace Leviathan
{
	Application* Application::m_instance = nullptr;

	Application* Application::Instance()
	{
		return m_instance;
	}

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

	weak_ptr<Renderer> Application::GetRenderer() const
	{
		return m_renderer;
	}

	int32 Application::Run() const
	{
		// Attempt to open the window
		if (m_window->Open())
		{
			Input::Create();
			GameTime::Init();

			m_renderer->Init(m_window);
			m_game->m_renderer = m_renderer;
			m_game->Init();

			// The window opened successfully, so run the render loop
			while (m_window->IsOpen())
			{
				GameTime::Tick();

				// Will return false if the application is Iconified
				if (!m_window->NewFrame())
				{
					continue;
				}

				m_game->Tick();
				m_game->m_ecsManager->Tick(); // TODO: Remove this

				m_game->Render();
				m_game->m_ecsManager->Render(); // TODO: Remove this

				// Present the current window and clear the input status
				m_renderer->Render();
				m_window->Present();
				Input::GetInstance()->ClearStatus();
			}

			m_game->Shutdown();

			// Close the window safely
			Input::Destroy();
			m_renderer->Shutdown();
			m_window->Close();

			return EXIT_SUCCESS;
		}

		return EXIT_FAILURE;
	}
}
