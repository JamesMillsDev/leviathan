#include <Core/Window.h>

#include <Utility/Config.h>

using Leviathan::Config;
using Leviathan::Window;

int main()
{
	Config* cfg = new Config{ "Engine" }; 
	Window* window = new Window{ cfg };
	if (window->Open())
	{
		while (window->IsOpen())
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