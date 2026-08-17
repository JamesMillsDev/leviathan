#pragma once

namespace Leviathan
{
	class GameInstance
	{
	public:
		virtual ~GameInstance() = default;

	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void Tick() = 0;
		virtual void Render() = 0;

	};
}