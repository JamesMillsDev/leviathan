#include "Core/Input.h"

#include <GLFW/glfw3.h>

namespace Leviathan
{
	Input* Input::m_instance = nullptr;

	Input* Input::GetInstance()
	{
		return m_instance;
	}

	bool Input::IsKeyDown(const EInputCodes key) const
	{
		const int32 inputId = static_cast<int32>(key);
		return m_currentKeys[inputId] == GLFW_PRESS;
	}

	bool Input::IsKeyUp(const EInputCodes key) const
	{
		const int32 inputId = static_cast<int32>(key);
		return m_currentKeys[inputId] == GLFW_RELEASE;
	}

	bool Input::WasKeyPressed(const EInputCodes key) const
	{
		const int32 inputId = static_cast<int32>(key);
		return m_currentKeys[inputId] == GLFW_PRESS && m_lastKeys[inputId] == GLFW_RELEASE;
	}

	bool Input::WasKeyReleased(const EInputCodes key) const
	{
		const int32 inputId = static_cast<int32>(key);

		return m_currentKeys[inputId] == GLFW_RELEASE && m_lastKeys[inputId] == GLFW_PRESS;
	}

	const TList<int32>& Input::GetPressedKeys() const
	{
		return m_pressedKeys;
	}

	const TList<uint32>& Input::GetPressedCharacters() const
	{
		return m_pressedCharacters;
	}

	bool Input::IsMouseButtonDown(const EInputCodes mouseButton) const
	{
		const int32 inputId = static_cast<int32>(mouseButton);

		return m_currentButtons[inputId] == GLFW_PRESS;
	}

	bool Input::IsMouseButtonUp(const EInputCodes mouseButton) const
	{
		const int32 inputId = static_cast<int32>(mouseButton);

		return m_currentButtons[inputId] == GLFW_RELEASE;
	}

	bool Input::WasMouseButtonPressed(const EInputCodes mouseButton) const
	{
		const int32 inputId = static_cast<int32>(mouseButton);

		return m_currentButtons[inputId] == GLFW_PRESS &&
			m_lastButtons[inputId] == GLFW_RELEASE;
	}

	bool Input::WasMouseButtonReleased(const EInputCodes inputMouseID) const
	{
		const int32 inputId = static_cast<int32>(inputMouseID);

		return m_currentButtons[inputId] == GLFW_RELEASE &&
			m_lastButtons[inputId] == GLFW_PRESS;
	}

	float Input::GetMouseX() const
	{
		return m_mouseX;
	}

	float Input::GetMouseY() const
	{
		return m_mouseY;
	}

	void Input::GetMouseXY(float* x, float* y) const
	{
		if (x != nullptr)
		{
			*x = m_mouseX;
		}

		if (y != nullptr)
		{
			*y = m_mouseY;
		}
	}

	float Input::GetMouseDeltaX() const
	{
		return m_mouseX - m_oldMouseX;
	}

	float Input::GetMouseDeltaY() const
	{
		return m_mouseY - m_oldMouseY;
	}

	void Input::GetMouseDelta(float* x, float* y) const
	{
		if (x != nullptr)
		{
			*x = m_mouseX - m_oldMouseX;
		}

		if (y != nullptr)
		{
			*y = m_mouseY - m_oldMouseY;
		}
	}

	float Input::GetMouseScroll() const
	{
		return m_mouseScroll;
	}

	void Input::Create()
	{
		m_instance = new Input();
	}

	void Input::Destroy()
	{
		delete m_instance;
	}

	void Input::ClearStatus()
	{
		m_pressedCharacters.Clear();

		GLFWwindow* window = glfwGetCurrentContext();

		m_pressedKeys.Clear();

		// Update keys
		for (int i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; ++i)
		{
			m_lastKeys[i] = m_currentKeys[i];

			if ((m_currentKeys[i] = glfwGetKey(window, i)) == GLFW_PRESS)
			{
				m_pressedKeys.Add(m_currentKeys[i]);
			}
		}

		// Update mouse
		for (int i = 0; i < 8; ++i)
		{
			m_lastButtons[i] = m_currentButtons[i];
			m_currentButtons[i] = glfwGetMouseButton(window, i);
		}

		// Update old mouse position
		m_oldMouseX = m_mouseX;
		m_oldMouseY = m_mouseY;
	}

	void Input::OnMouseMove(const float newXPos, const float newYPos)
	{
		m_mouseX = newXPos;
		m_mouseY = newYPos;

		if (m_firstMouseMove)
		{
			// On first move after Startup/entering window reset old mouse position
			m_oldMouseX = newXPos;
			m_oldMouseY = newYPos;
			m_firstMouseMove = false;
		}
	}

	Input::Input()
		: m_oldMouseX{ 0 }, m_oldMouseY{ 0 }, m_firstMouseMove{ false },
		m_lastKeys{ new int32[GLFW_KEY_LAST + 1] }, m_currentKeys{ new int32[GLFW_KEY_LAST + 1] }
	{
		GLFWwindow* window = glfwGetCurrentContext();

		for (int32 i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; ++i)
		{
			m_lastKeys[i] = m_currentKeys[i] = glfwGetKey(window, i);
		}

		for (int32 i = 0; i < MOUSE_BUTTON_COUNT; ++i)
		{
			m_lastButtons[i] = m_currentButtons[i] = glfwGetMouseButton(window, i);
		}

		auto KeyPressCallback = [](GLFWwindow* window, const int32 key, const int32 scancode, const int32 action, const int32 mods)
			{
				for (auto& f : GetInstance()->m_keyCallbacks)
				{
					f(window, key, scancode, action, mods);
				}
			};

		auto CharacterInputCallback = [](GLFWwindow* window, const uint32 character)
			{
				GetInstance()->m_pressedCharacters.Add(character);

				for (auto& f : GetInstance()->m_charCallbacks)
				{
					f(window, character);
				}
			};

		auto MouseMoveCallback = [](GLFWwindow* window, const double x, const double y)
			{
				int w = 0, h = 0;
				glfwGetWindowSize(window, &w, &h);

				GetInstance()->OnMouseMove(static_cast<float>(x), static_cast<float>(h) - static_cast<float>(y));

				for (auto& f : GetInstance()->m_mouseMoveCallbacks)
				{
					f(window, x, h - y);
				}
			};

		auto MouseInputCallback = [](GLFWwindow* window, const int32 button, const int32 action, const int32 mods)
			{
				for (auto& f : GetInstance()->m_mouseButtonCallbacks)
				{
					f(window, button, action, mods);
				}
			};

		auto MouseScrollCallback = [](GLFWwindow* window, const double xOffset, const double yOffset)
			{
				GetInstance()->m_mouseScroll += static_cast<float>(yOffset);

				for (auto& f : GetInstance()->m_mouseScrollCallbacks)
				{
					f(window, xOffset, yOffset);
				}
			};

		auto MouseEnterCallback = [](GLFWwindow* window, [[maybe_unused]] int32 entered)
			{
				// Set flag to prevent large mouse delta on entering screen
				GetInstance()->m_firstMouseMove = true;
			};

		glfwSetKeyCallback(window, KeyPressCallback);
		glfwSetCharCallback(window, CharacterInputCallback);
		glfwSetMouseButtonCallback(window, MouseInputCallback);
		glfwSetCursorPosCallback(window, MouseMoveCallback);
		glfwSetScrollCallback(window, MouseScrollCallback);
		glfwSetCursorEnterCallback(window, MouseEnterCallback);

		m_mouseX = 0;
		m_mouseY = 0;
		m_mouseScroll = 0;
	}

	Input::~Input()
	{
		delete[] m_lastKeys;
		delete[] m_currentKeys;
	}
}
