#if _DEBUG
#include <crtdbg.h>
#endif

#include <Core/Application.h>

#include "Editor/EditorGameInstance.h"

int main()
{
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	return Leviathan::Application::Open<Leviathan::EditorGameInstance>();
}
