#include "Graphics/Resources/ResourceManager.h"

#include <ranges>

namespace Leviathan
{
	ResourceManager::ResourceManager(PrivateKey) {}

	void ResourceManager::Clean()
	{
		for (const IResource* resource : m_resources | std::ranges::views::values)
		{
			delete resource;
		}
	}
}
