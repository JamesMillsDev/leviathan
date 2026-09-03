#pragma once

#include <queue>

#include "Gameplay/ECS/Types.h"
#include "Maths/Alias.h"
#include "Utility/Collections/TArray.h"

using std::queue;

namespace Leviathan
{
	class EntityManager
	{
		friend class ECSManager;

	private:
		/** Unused Entity ID's */
		queue<Entity> m_availableIds;
		/** Array of signatures where the index corresponds to the entity ID. */
		TArray<Signature, MAX_ENTITIES> m_signatures;
		/** Total living entities - used to keep limits on how many exist */
		uint32 m_livingEntityCount;

	private:
		EntityManager();

	public:
		Entity MakeEntity();
		void DestroyEntity(Entity entity);

		void SetSignature(Entity entity, Signature signature);
		Signature GetSignature(Entity entity);

	};
}
