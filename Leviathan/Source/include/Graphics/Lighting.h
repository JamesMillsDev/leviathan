#pragma once

#include <memory>
#include <glm/vec3.hpp>

#include "Utility/Collections/TList.h"

using std::shared_ptr;
using glm::vec3;

extern int debugMode;

namespace Leviathan
{
	class Config;
	class GBuffer;
	class Light;
	class FrameBuffer;
	class Material;
	class Mesh;
	class Shader;

	class Lighting
	{
		friend class Renderer;

	private:
		TList<Light*> m_lights;

		Shader* m_shader;
		Material* m_material;

		FrameBuffer* m_shadowMap;

	private:
		explicit Lighting(const shared_ptr<Config>& config);
		~Lighting();

	private:
		void Render(Mesh* screenQuad, GBuffer* gBuffer, const vec3& viewLoc);

	};
}
