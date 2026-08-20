#include <Core/Application.h>

#include "Editor/EditorGameInstance.h"

using namespace Leviathan;

int main()
{
	return Application::Open<EditorGameInstance>();
}
