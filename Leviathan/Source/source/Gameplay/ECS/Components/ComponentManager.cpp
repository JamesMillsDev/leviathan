#include "Gameplay/ECS/Components/ComponentManager.h"

namespace Leviathan
{
	ComponentManager::ComponentManager() :
		m_nextComponentType{ 0 }
	{}

	void ComponentManager::OnEntityDestroyed(const Entity entity)
	{
		// Notify each component array that an entity has been destroyed
		// If it has a component for that entity, it will remove it
		for (TMapEntry<const char*, shared_ptr<IComponentArray>>* pair : m_componentArrays)
		{
			auto const& component = pair->Value();

			component->OnEntityDestroyed(entity);
		}

	}
}
