#pragma once

#include <bitset>

#include "Maths/Alias.h"

namespace Leviathan
{
	constexpr uint32 MAX_ENTITIES = 5000;
	constexpr uint8 MAX_COMPONENTS = 32;

	using Entity = uint32;
	using ComponentType = uint8;
	using Signature = std::bitset<MAX_COMPONENTS>;
}
