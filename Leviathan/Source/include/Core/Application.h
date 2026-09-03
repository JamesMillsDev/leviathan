#pragma once

#if _DEBUG
#include <crtdbg.h>
#endif

#include <memory>

#include "Maths/Alias.h"

using std::shared_ptr;
using std::weak_ptr;

namespace Leviathan
{
	class Config;
	class GameInstance;
	class Renderer;
	class ResourceManager;
	class Window;

    class Application
    {
    private:
        static Application* m_instance;

    public:
        template<typename T>
        static int32 Open();

        static Application* Instance();

    private:
        shared_ptr<Config> m_engineConfig;
        shared_ptr<Window> m_window;
        shared_ptr<GameInstance> m_game;
        shared_ptr<Renderer> m_renderer;
        shared_ptr<ResourceManager> m_resourceManager;

    private:
        Application();

    public:
        [[nodiscard]] weak_ptr<Window> GetWindow() const;
        [[nodiscard]] weak_ptr<GameInstance> GetGameInstance() const;
        [[nodiscard]] weak_ptr<Renderer> GetRenderer() const;
        [[nodiscard]] weak_ptr<ResourceManager> GetResourceManager() const;

    private:
        int32 Run() const;

    };

    template <typename T>
    int32 Application::Open()
    {
        static_assert(std::is_base_of_v<GameInstance, T>, "Provided type is not a Game Instance!");

        if (m_instance != nullptr)
        {
            return EXIT_FAILURE;
        }

    #if _DEBUG
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

        m_instance = new Application;
        m_instance->m_game = std::make_shared<T>();

        const int32 exitCode = m_instance->Run();
        delete m_instance;
        m_instance = nullptr;

	    return exitCode;
    }
}
