#include "Graphics/Material.h"

#include "Graphics/Shader.h"

namespace Leviathan
{
	Material::Material(Shader* shader)
		: m_shader{ shader }
	{
		
	}

	bool Material::Bind() const
	{
		return m_shader->Bind();
	}
}
