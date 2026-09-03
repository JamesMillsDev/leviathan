#include "Gameplay/ECS/Systems/SystemManager.h"

#include "Gameplay/ECS/Systems/System.h"

namespace Leviathan
{
	void SystemManager::OnEntityDestroyed(const Entity entity)
	{
		// Erase a destroyed entity from all system lists
		// mEntities is a set so no check needed
		for (TMapEntry<const char*, shared_ptr<System>>* pair : m_systems)
		{
			const shared_ptr<System> system = pair->Value();

			system->m_entities.erase(entity);
		}
	}

	void SystemManager::OnEntitySignatureChanged(Entity entity, Signature entitySignature)
	{
		// Notify each system that an entity's signature changed
		for (TMapEntry<const char*, shared_ptr<System>>* pair : m_systems)
		{
			const char* type = pair->Key();
			const shared_ptr<System> system = pair->Value();
			Signature systemSignature = m_signatures[type];

			// Entity signature matches system signature - insert into set
			if ((entitySignature & systemSignature) == systemSignature)
			{
				system->m_entities.insert(entity);
			}
			// Entity signature does not match system signature - erase from set
			else
			{
				system->m_entities.erase(entity);
			}
		}

	}
}
