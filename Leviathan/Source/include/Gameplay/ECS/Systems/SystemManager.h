#pragma once

#include <cassert>
#include <memory>

#include "Gameplay/ECS/Types.h"
#include "Utility/Collections/TMap.h"

using std::shared_ptr;

namespace Leviathan
{
	class System;

	class SystemManager
	{
		friend class ECSManager;

	private:
		/** Map from system type string pointer to a signature. */
		TMap<const char*, Signature> m_signatures;

		/** Map from system type string pointer to a system pointer. */
		TMap<const char*, shared_ptr<System>> m_systems;

	private:
		SystemManager() = default;

	public:
		template<typename T>
		shared_ptr<T> RegisterSystem()
		{
			const char* typeName = typeid(T).name();

			assert(!m_systems.ContainsKey(typeName) && "Registering system more than once.");

			// Create a pointer to the system and return it so it can be used externally
			auto system = std::make_shared<T>();
			m_systems.Add(typeName, system);
			return system;
		}

		template<typename T>
		void SetSignature(Signature signature)
		{
			const char* typeName = typeid(T).name();

			assert(m_systems.ContainsKey(typeName) && "System used before registered.");

			// Set the signature for this system
			m_signatures.Add(typeName, signature);
		}

		void OnEntityDestroyed(Entity entity);

		void OnEntitySignatureChanged(Entity entity, Signature entitySignature);

	};
}
