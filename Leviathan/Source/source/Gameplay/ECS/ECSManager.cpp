#include "Gameplay/ECS/ECSManager.h"

#include "Gameplay/ECS/Systems/System.h"

namespace Leviathan
{
	Entity ECSManager::MakeEntity() const
	{
		return m_entityManager->MakeEntity();
	}

	void ECSManager::DestroyEntity(const Entity entity) const
	{
		m_entityManager->DestroyEntity(entity);
		m_systemManager->OnEntityDestroyed(entity);
		m_componentManager->OnEntityDestroyed(entity);
	}

	void ECSManager::Init()
	{
		m_componentManager = shared_ptr<ComponentManager>{ new ComponentManager };
		m_entityManager = shared_ptr<EntityManager>{ new EntityManager };
		m_systemManager = shared_ptr<SystemManager>{ new SystemManager };
	}

	void ECSManager::Tick(const float dt) const
	{
		for (TMapEntry<const char*, shared_ptr<System>>* system : m_systemManager->m_systems)
		{
			system->Value()->Tick(dt, m_componentManager);
		}
	}
}
