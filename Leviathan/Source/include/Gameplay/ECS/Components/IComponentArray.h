#pragma once

#include <cassert>
#include <unordered_map>

#include "Gameplay/ECS/Types.h"
#include "Maths/Alias.h"
#include "Utility/Collections/TArray.h"

using std::unordered_map;

namespace Leviathan
{
	/**
	 * The one instance of virtual inheritance in the entire implementation.
	 * An interface is needed so that the ComponentManager can tell a generic
	 * ComponentArray that an entity has been destroyed and that it needs to
	 * update its array mappings.
	 */
	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;

	public:
		virtual void OnEntityDestroyed(Entity entity) = 0;

	};

	template<typename T>
	class ComponentArray : public IComponentArray
	{
	private:
		/**
		 * The packed array of components (of generic type T), set to a specified
		 * maximum amount, matching the maximum number of entities allowed to exist
		 * simultaneously, so that each entity has a unique spot.
		 */
		TArray<T, MAX_ENTITIES> m_componentArray;

		/** Map from an entity ID to an array index. */
		unordered_map<Entity, uint64> m_entityToIndexMap;

		/** Map from an array index to an entity ID. */
		unordered_map<uint64, Entity> m_indexToEntityMap;

		/** Total size of valid entries in the array. */
		uint64 m_size;

	public:
		ComponentArray() :
			m_size{ 0 }
		{}

	public:
		void InsertData(const Entity entity, T component)
		{
			assert(!m_entityToIndexMap.contains(entity) && "Component added to same entity more than once.");

			// Put new entry at end and update the maps
			size_t newIndex = m_size;
			m_entityToIndexMap[entity] = newIndex;
			m_indexToEntityMap[newIndex] = entity;
			m_componentArray[newIndex] = component;
			++m_size;
		}

		void RemoveData(const Entity entity)
		{
			assert(m_entityToIndexMap.contains(entity) && "Removing non-existent component.");

			// Copy element at end into deleted element's place to maintain density
			size_t indexOfRemovedEntity = m_entityToIndexMap[entity];
			size_t indexOfLastElement = m_size - 1;
			m_componentArray[indexOfRemovedEntity] = m_componentArray[indexOfLastElement];

			// Update map to point to moved spot
			const Entity entityOfLastElement = m_indexToEntityMap[indexOfLastElement];
			m_entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
			m_indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

			m_entityToIndexMap.erase(entity);
			m_indexToEntityMap.erase(indexOfLastElement);

			--m_size;
		}

		T& GetData(const Entity entity)
		{
			assert(m_entityToIndexMap.contains(entity) && "Retrieving non-existent component.");

			// Return a reference to the entity's component
			return m_componentArray[m_entityToIndexMap[entity]];
		}

		void OnEntityDestroyed(const Entity entity) override
		{
			if (m_entityToIndexMap.contains(entity))
			{
				// Remove the entity's component if it existed
				RemoveData(entity);
			}
		}

	};
}
