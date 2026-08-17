#pragma once

#include <Maths/Color.h>

struct GLFWwindow;

namespace Leviathan
{
	class Window
	{
	private:
		GLFWwindow* m_window;
		int32 m_width;
		int32 m_height;
		const char* m_title;
		Color m_clearColor;

	public:
		Window(int32 w, int32 h, const char* title, const Color& clrColor);

	public:
		bool Open();
		void Close() const;

		bool NewFrame() const;
		void Present() const;

	};
}
