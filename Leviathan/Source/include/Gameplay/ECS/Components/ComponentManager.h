#pragma once

#include <memory>

#include "Gameplay/ECS/Types.h"
#include "Gameplay/ECS/Components/IComponentArray.h"
#include "Utility/Collections/TMap.h"

using std::shared_ptr;

namespace Leviathan
{
	class ComponentManager
	{
		friend class ECSManager;

	private:
		struct PrivateKey
		{};

	private:
		/** Map from type string pointer to a component type. */
		TMap<const char*, ComponentType> m_componentTypes;

		/** Map from type string pointer to a component array. */
		TMap<const char*, shared_ptr<IComponentArray>> m_componentArrays;

		/** The component type to be assigned to the next registered component - starting at 0. */
		ComponentType m_nextComponentType;

	public:
		ComponentManager();

	public:
		template<typename T>
		void RegisterComponent()
		{
			const char* typeName = typeid(T).name();

			assert(!m_componentTypes.ContainsKey(typeName) && "Registering component type more than once.");

			// Add this component type to the component type map
			m_componentTypes.Add(typeName, m_nextComponentType);

			// Create a ComponentArray pointer and add it to the component arrays map
			m_componentArrays.Add(typeName, std::make_shared<ComponentArray<T>>());

			// Increment the value so that the next component registered will be different
			++m_nextComponentType;

		}

		template<typename T>
		ComponentType GetComponentType()
		{
			const char* typeName = typeid(T).name();

			assert(m_componentTypes.ContainsKey(typeName) && "Component not registered before use.");

			// Return this component's type - used for creating signatures
			return m_componentTypes[typeName];
		}

		template<typename T>
		void AddComponent(Entity entity, T component)
		{
			// Add a component to the array for an entity
			GetComponentArray<T>()->InsertData(entity, component);
		}

		template<typename T>
		void RemoveComponent(Entity entity)
		{
			// Remove a component from the array for an entity
			GetComponentArray<T>()->RemoveData(entity);
		}

		template<typename T>
		T& GetComponent(Entity entity)
		{
			// Get a reference to a component from the array for an entity
			return GetComponentArray<T>()->GetData(entity);
		}

		void OnEntityDestroyed(Entity entity);

	private:
		template<typename T>
		shared_ptr<ComponentArray<T>> GetComponentArray()
		{
			const char* typeName = typeid(T).name();

			assert(m_componentTypes.ContainsKey(typeName) && "Component not registered before use.");

			return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[typeName]);

		}

	};
}
