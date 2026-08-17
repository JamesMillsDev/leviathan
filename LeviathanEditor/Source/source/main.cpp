#include <Core/Window.h>

#include "GLFW/glfw3.h"

using Leviathan::Color;
using Leviathan::Window;

int main()
{
	Window* window = new Window{ 800, 600, "Leviathan Editor", Color::GREEN };
	if (window->Open())
	{
		GLFWwindow* win = glfwGetCurrentContext();
		
		while (!glfwWindowShouldClose(win))
		{
			if (!window->NewFrame())
			{
				continue;
			}

			window->Present();
		}

		window->Close();
	}

	delete window;

	return 0;
}