#pragma once

#include <Maths/Color.h>

struct GLFWwindow;

namespace Leviathan
{
	class Config;

	class Window
	{
	private:
		GLFWwindow* m_window;
		int32 m_width;
		int32 m_height;
		string m_title;
		Color m_clearColor;

	public:
		explicit Window(Config* config);

	public:
		[[nodiscard]] bool IsOpen() const;

		[[nodiscard]] bool Open();
		void Close() const;

		[[nodiscard]] bool NewFrame() const;
		void Present() const;

	};
}
