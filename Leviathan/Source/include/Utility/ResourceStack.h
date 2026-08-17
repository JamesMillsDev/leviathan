#pragma once

#include <functional>

#include "Utility/Collections/TStack.h"

namespace Leviathan
{
	using CleanupFunction = std::function<void()>;

	class ResourceStack : public TStack<CleanupFunction>
	{
	public:
		~ResourceStack() override;

	};
}