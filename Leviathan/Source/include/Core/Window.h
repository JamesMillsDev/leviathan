#pragma once

#include <memory>

#include <Maths/Color.h>

struct GLFWwindow;

using std::weak_ptr;

namespace Leviathan
{
	class Config;

	class Window
	{
		friend class Application;

	private:
		struct PrivateKey
		{
		};

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

	private:
		[[nodiscard]] bool Open();
		void Close() const;

		[[nodiscard]] bool NewFrame() const;
		void Present() const;

	};
}
