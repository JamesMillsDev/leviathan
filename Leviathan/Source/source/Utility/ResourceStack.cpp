#include "Utility/ResourceStack.h"

namespace Leviathan
{
	ResourceStack::~ResourceStack()
	{
		while (m_top > -1)
		{
			m_stack[m_top--]();
		}
	}
}