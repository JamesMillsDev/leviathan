#include "Gameplay/ECS/ECSManager.h"

#include "Gameplay/ECS/Systems/System.h"

namespace Leviathan
{
	ECSManager::ECSManager() 
	{
		m_componentManager = shared_ptr<ComponentManager>{ new ComponentManager };
		m_entityManager = shared_ptr<EntityManager>{ new EntityManager };
		m_systemManager = shared_ptr<SystemManager>{ new SystemManager };
	}

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

	void ECSManager::Tick() const
	{
		for (TMapEntry<const char*, shared_ptr<System>>* system : m_systemManager->m_systems)
		{
			system->Value()->Tick(m_componentManager);
		}
	}

	void ECSManager::Render(const shared_ptr<Renderer>& renderer) const
	{
		for (TMapEntry<const char*, shared_ptr<System>>* system : m_systemManager->m_systems)
		{
			system->Value()->Render(m_componentManager, renderer);
		}
	}
}
