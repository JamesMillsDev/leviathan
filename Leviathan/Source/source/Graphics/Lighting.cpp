#include "Graphics/Lighting.h"

#include <format>

#include <glm/ext/matrix_clip_space.hpp>

#include "glad/gl.h"

#include "Graphics/GBuffer.h"
#include "Graphics/Light.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Textures/FrameBuffer.h"

#include "Utility/Config.h"

namespace Leviathan
{
	Lighting::Lighting(const shared_ptr<Config>& config)
		: m_shader{ new Shader{ "Shaders/deferred_lit" } }
	{
		m_material = new Material{ m_shader };
		m_shadowMap = new FrameBuffer{
			config->Get<int32>("Shadows.Map.Width"), config->Get<int32>("Shadows.Map.Height"),
			GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER
		};
	}

	Lighting::~Lighting()
	{
		delete m_shadowMap;
		delete m_material;
		delete m_shader;
	}

	void Lighting::Render(Mesh* screenQuad, GBuffer* gBuffer, const vec3& viewLoc)
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
		m_material->SetMaterialProperties(m_shadowMap->Handle(), true);
		m_material->Set("gBuffer.debugPhase", 3);

		gBuffer->Bind(m_material);

		screenQuad->Render();
	}
}
