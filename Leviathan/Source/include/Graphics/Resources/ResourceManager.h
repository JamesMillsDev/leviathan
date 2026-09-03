#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Resource.h"

using std::string;
using std::unordered_map;

namespace Leviathan
{
	class ResourceManager
	{
		friend class Application;

	private:
		struct PrivateKey
		{};

	private:
		unordered_map<string, IResource*> m_resources;

	public:
		explicit ResourceManager(PrivateKey);

	public:
		template<typename T>
		T* Find(const string& id)
		{
			if (!m_resources.contains(id))
			{
				return nullptr;
			}

			return dynamic_cast<Resource<T>*>(m_resources[id])->Get();
		}

		template<typename T>
		void Register(const string& id, T* resource)
		{
			if (m_resources.contains(id))
			{
				return;
			}

			m_resources[id] = new Resource<T>(resource);
		}

	private:
		void Clean();

	};
}
