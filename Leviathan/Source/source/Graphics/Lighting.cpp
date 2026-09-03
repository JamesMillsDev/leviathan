#include "Graphics/Lighting.h"

#include <format>

#include <glm/ext/matrix_clip_space.hpp>

#include "Graphics/GBuffer.h"
#include "Graphics/Light.h"
#include "Graphics/Resources/Material.h"
#include "Graphics/Resources/Mesh.h"

#include "Utility/Config.h"

int debugMode = 0;

namespace Leviathan
{
	Lighting::Lighting(const shared_ptr<Config>& config)
		: m_shader{ new Shader{ "Shaders/deferred_lit" } }
	{
		m_material = new Material{ m_shader };
	}

	Lighting::~Lighting()
	{
		delete m_material;
		delete m_shader;
	}

	void Lighting::Render(Mesh* screenQuad, GBuffer* gBuffer, const vec3& viewLoc, const uint32 shadowMapHandle)
	{
		if (!m_material->Bind())
		{
			return;
		}

		m_material->Set("lightCount", static_cast<int32>(m_lights.Count()));
		const mat4 lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 7.5f);

		for (int32 i = 0; i < static_cast<int32>(m_lights.Count()); ++i)
		{
			m_lights[i]->SetMaterialProperties(m_material, i);

			mat4 lightView = glm::inverse(m_lights[i]->transform);
			m_material->Set(std::format("lightSpaceMatrices[{}]", i), lightProjection * lightView);
		} 

		m_material->Set(m_material->m_cameraLocationLoc, viewLoc);
		m_material->SetMaterialProperties(shadowMapHandle, true, gBuffer->Bind(m_material));
		m_material->Set("gBuffer.debugPhase", debugMode);

		screenQuad->Render();
	}
}
