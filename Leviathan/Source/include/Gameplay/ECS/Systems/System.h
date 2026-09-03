#pragma once

#include <memory>
#include <set>

#include "Gameplay/ECS/Types.h"

using std::shared_ptr;
using std::set;

namespace Leviathan
{
	class ComponentManager;

	class System
	{
		friend class SystemManager;

	protected:
		set<Entity> m_entities;

	public:
		virtual ~System() = default;

	public:
		virtual Signature GetSystemSignature(const shared_ptr<ComponentManager>& componentManager) = 0;
		virtual void Tick(const shared_ptr<ComponentManager>& componentManager) {}
		virtual void Render(const shared_ptr<ComponentManager>& componentManager) {}

	};
}
