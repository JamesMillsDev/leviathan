#pragma once

#include <memory>

#include "ECS/ECSManager.h"

using std::shared_ptr;

namespace Leviathan
{
	class Renderer;

	class GameInstance
	{
		friend class Application;

	protected:
		shared_ptr<Renderer> m_renderer;
		shared_ptr<ECSManager> m_ecsManager;

	public:
		GameInstance()
		{
			m_ecsManager->Init();
		}

		virtual ~GameInstance() = default;

	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void Tick() = 0;
		virtual void Render() = 0;

	};
}