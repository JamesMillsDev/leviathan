#pragma once

#include <memory>

#include "Gameplay/ECS/Systems/System.h"

using std::shared_ptr;

namespace Leviathan
{
	class Renderer;

	class RenderSystem : public System
	{
	public:
		shared_ptr<Renderer> renderer;

	public:
		Signature GetSystemSignature(const shared_ptr<ComponentManager>& componentManager) override;
		void Render(const shared_ptr<ComponentManager>& componentManager) override;

	};
}