#pragma once

#include <functional>
#include <memory>

#include <Maths/Color.h>

#include "Utility/Collections/TList.h"

struct GLFWwindow;

using std::weak_ptr;
using WindowResizeCallback = std::function<void(int32, int32)>;

extern void FramebufferSizeCallback(GLFWwindow*, int32 width, int32 height);

namespace Leviathan
{
	class Config;

	class Window
	{
		friend class Application;
		friend class Renderer;
		friend void ::FramebufferSizeCallback(GLFWwindow*, int32 width, int32 height);

	private:
		struct PrivateKey
		{
		};

	public:
		TList<WindowResizeCallback> onWindowResized;

	private:
		GLFWwindow* m_window;
		int32 m_width;
		int32 m_height;
		string m_title;
		Color m_clearColor;

	public:
		explicit Window(const weak_ptr<Config>& config, PrivateKey key);

	public:
		[[nodiscard]] bool IsOpen() const;

		void Clear(uint32 layer) const;

		[[nodiscard]] int32 Width() const;
		[[nodiscard]] int32 Height() const;

	private:
		[[nodiscard]] bool Open();
		void Close() const;

		[[nodiscard]] bool NewFrame() const;
		void Present() const;

	};
}
