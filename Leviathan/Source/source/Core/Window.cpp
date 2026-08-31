#include "Core/Window.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <Utility/Config.h>
#include <Utility/Console.h>

#include "Core/Application.h"

void FramebufferSizeCallback(GLFWwindow* _, const int32 width, const int32 height)
{
	if (auto win = Leviathan::Application::Instance()->GetWindow().lock())
	{
		win->m_width = width;
		win->m_height = height;
	}

	glViewport(0, 0, width, height);

#if IS_EDITOR
	glScissor(0, 0, width, height);
#endif
}

namespace Leviathan
{
	Window::Window(const weak_ptr<Config>& config, PrivateKey key) :
		m_window{ nullptr }
	{
		if (const auto cfg = config.lock())
		{
			m_width = cfg->Get<int32>("Window.Width");
			m_height = cfg->Get<int32>("Window.Height");
			m_title = cfg->Get<string>("Window.Title");
			m_clearColor = cfg->Get<Color>("Window.ClrColor");
		}
	}

	bool Window::IsOpen() const
	{
		return !glfwWindowShouldClose(m_window);
	}

	void Window::Clear(const uint32 layer) const
	{
		glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
		glClear(layer);
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
		glEnable(GL_FRAMEBUFFER_SRGB);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		
		#if IS_EDITOR
		glEnable(GL_SCISSOR_TEST);
		#endif

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

		return true;
	}

	void Window::Present() const
	{
		glfwSwapBuffers(m_window);
	}
}
