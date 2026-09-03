#pragma once

#include <memory>

#include "Gameplay/ECS/EntityManager.h"
#include "Gameplay/ECS/Types.h"
#include "Gameplay/ECS/Components/ComponentManager.h"
#include "Gameplay/ECS/Systems/System.h"
#include "Gameplay/ECS/Systems/SystemManager.h"

using std::shared_ptr;

namespace Leviathan
{
	class ECSManager
	{
		friend class GameInstance;
		friend class Application; // TODO: Remove this

	private:
		shared_ptr<ComponentManager> m_componentManager;
		shared_ptr<EntityManager> m_entityManager;
		shared_ptr<SystemManager> m_systemManager;

	private:
		ECSManager();

	public:
		Entity MakeEntity() const;
		void DestroyEntity(Entity entity) const;

		template<typename T>
		void RegisterComponent() const
		{
			m_componentManager->RegisterComponent<T>();
		}

		template<typename T>
		void AddComponent(const Entity entity, T component)
		{
			m_componentManager->AddComponent<T>(entity, component);

			Signature signature = m_entityManager->GetSignature(entity);
			signature.set(m_componentManager->GetComponentType<T>(), true);
			m_entityManager->SetSignature(entity, signature);

			m_systemManager->OnEntitySignatureChanged(entity, signature);
		}

		template<typename T>
		void RemoveComponent(const Entity entity) const
		{
			m_componentManager->RemoveComponent<T>(entity);

			Signature signature = m_entityManager->GetSignature(entity);
			signature.set(m_componentManager->GetComponentType<T>(), false);
			m_entityManager->SetSignature(entity, signature);

			m_systemManager->OnEntitySignatureChanged(entity, signature);
		}

		template<typename T>
		T& GetComponent(const Entity entity)
		{
			return m_componentManager->GetComponent<T>(entity);
		}

		template<typename T>
		ComponentType GetComponentType() const
		{
			return m_componentManager->GetComponentType<T>();
		}

		template<typename T>
		shared_ptr<T> RegisterSystem() const
		{
			const shared_ptr<System> system = m_systemManager->RegisterSystem<T>();
			SetSystemSignature<T>(system->GetSystemSignature(m_componentManager));

			return std::static_pointer_cast<T>(system);
		}

		template<typename T>
		void SetSystemSignature(const Signature signature) const
		{
			m_systemManager->SetSignature<T>(signature);
		}

	private:
		void Tick() const;
		void Render() const;

	};
}