#pragma once

#include <chrono>

#include "Maths/Alias.h"

using std::chrono::steady_clock;

namespace Leviathan
{
	class GameTime
	{
	public:
		friend class Application;

	public:
		static float DeltaTime();
		static float AppTime();
		static uint32 FPS();

	private:
		static steady_clock::time_point m_currTime;
		static steady_clock::time_point m_prevTime;

		static steady_clock::time_point m_initialTime;

		static float m_deltaTime;
		static uint32 m_frames;
		static float m_fpsInterval;
		static uint32 m_fps;

	private:
		static void Init();
		static void Tick();
	};
}