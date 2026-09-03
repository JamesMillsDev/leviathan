#pragma once

namespace Leviathan
{
	class Material;
	class Mesh;

	struct RenderComponent
	{
		Mesh* mesh;
		Material* material;
	};
}