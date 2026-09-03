#include "Gameplay/ECS/EntityManager.h"

#include <cassert>

namespace Leviathan
{
	EntityManager::EntityManager() :
		m_livingEntityCount{ 0 }
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
		{
			m_availableIds.push(entity);
		}
	}

	Entity EntityManager::MakeEntity()
	{
		assert(m_livingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

		// Take an id from the available queue
		Entity id = m_availableIds.front();
		m_availableIds.pop();
		++m_livingEntityCount;

		return id;
	}

	void EntityManager::DestroyEntity(Entity entity)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		// Invalidate the destroyed entity's signature
		m_signatures[entity].reset();

		// Put the destroyed id back into the queue
		m_availableIds.push(entity);
		--m_livingEntityCount;
	}

	void EntityManager::SetSignature(Entity entity, Signature signature)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		// Put this entity's signature into the array
		m_signatures[entity] = signature;
	}

	Signature EntityManager::GetSignature(Entity entity)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		// Get this entity's signature from the array
		return m_signatures[entity];
	}
}
