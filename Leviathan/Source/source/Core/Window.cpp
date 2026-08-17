#include "Core/Window.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <Utility/Config.h>
#include <Utility/Console.h>

namespace
{
	void FramebufferSizeCallback(GLFWwindow* _, const int32 width, const int32 height)
	{
		glScissor(0, 0, width, height);
		glViewport(0, 0, width, height);
	}
}

namespace Leviathan
{
	Window::Window(Config* config) :
		m_window{ nullptr }, m_width{ config->Get<int32>("Window.Width") },
		m_height{ config->Get<int32>("Window.Height") }, m_title{ config->Get<string>("Window.Title") },
		m_clearColor{ config->Get<Color>("Window.ClrColor") }
	{

	}

	bool Window::IsOpen() const
	{
		return !glfwWindowShouldClose(m_window);
	}

	bool Window::Open()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
		if (m_window == nullptr)
		{
			Console::Error("Failed to create GLFW window!");
			glfwTerminate();
			return false;
		}

		glfwMakeContextCurrent(m_window);
		if (!gladLoadGL(glfwGetProcAddress))
		{
			Console::Error("Failed initialise Glad!");
			glfwTerminate();
			return false;
		}

		glViewport(0, 0, m_width, m_height);
		glEnable(GL_SCISSOR_TEST);
		glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
		return true;
	}

	void Window::Close() const
	{
		glfwTerminate();
		glfwDestroyWindow(m_window);
	}

	bool Window::NewFrame() const
	{
		glfwPollEvents();

		if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED))
		{
			return false;
		}

		glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT);

		return true;
	}

	void Window::Present() const
	{
		glfwSwapBuffers(m_window);
	}
}
