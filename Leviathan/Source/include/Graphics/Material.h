#pragma once

namespace Leviathan
{
	class Shader;

	class Material
	{
	private:
		Shader* m_shader;

	public:
		Material(Shader* shader);

	public:
		bool Bind() const;

	};
}