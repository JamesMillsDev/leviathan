#pragma once

#include <memory>

#include "Maths/Alias.h"

using std::shared_ptr;
using std::weak_ptr;

namespace Leviathan
{
	class Config;
	class GameInstance;
	class Window;

    class Application
    {
    private:
        shared_ptr<Config> m_engineConfig;
        shared_ptr<Window> m_window;
        shared_ptr<GameInstance> m_game;

    public:
        Application();

    public:
        [[nodiscard]] weak_ptr<Window> GetWindow() const;
        [[nodiscard]] weak_ptr<GameInstance> GetGameInstance() const;

        int32 Run() const;

    };
}