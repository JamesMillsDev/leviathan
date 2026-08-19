#pragma once

#include <memory>

using std::shared_ptr;

namespace Leviathan
{
	class Renderer;

	class GameInstance
	{
		friend class Application;

	protected:
		shared_ptr<Renderer> m_renderer;

	public:
		virtual ~GameInstance() = default;

	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void Tick() = 0;
		virtual void Render() = 0;

	};
}