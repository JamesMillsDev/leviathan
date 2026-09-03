#include "Graphics/Shadows.h"

#include <glad/gl.h>
#include <glm/ext/matrix_clip_space.hpp>

#include "Core/Window.h"
#include "Graphics/Light.h"
#include "Graphics/Resources/Material.h"
#include "Graphics/Resources/Mesh.h"
#include "Graphics/Resources/Shader.h"
#include "Graphics/Resources/Textures/FrameBuffer.h"
#include "Utility/Config.h"

namespace Leviathan
{
	Shadows::Shadows(const shared_ptr<Window>& window, const shared_ptr<Config>& config) :
		m_window{ window }, m_bound{ false }, m_shader{ new Shader{"Shaders/depth"} },
		m_material{ nullptr }
	{
		m_material = new Material{ m_shader };
		const int32 w = config->Get<int32>("Shadows.Map.Width");
		const int32 h = config->Get<int32>("Shadows.Map.Height");

		m_shadowMap = new FrameBuffer{
			w, h, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, GL_FLOAT, GL_NEAREST,
			GL_CLAMP_TO_BORDER
		};
	}

	Shadows::~Shadows()
	{
		delete m_shadowMap;
		delete m_material;
		delete m_shader;
	}

	void Shadows::BeginRecording()
	{
		if (m_bound)
		{
			return;
		}

		m_shadowMap->Bind();
		m_bound = true;
	}

	void Shadows::QueueRender(Mesh* mesh, const mat4& transform)
	{
		m_renders.push([this, transform, mesh](const mat4& projection, const mat4& view)
			{
				if (!m_material->Bind())
				{
					return;
				}

				m_material->Set(m_material->m_vpLoc, projection * view);
				m_material->Set(m_material->m_modelLoc, transform);

				mesh->Render();
			});
	}

	void Shadows::Render(const TList<Light*>& lights)
	{
		BeginRecording();
		m_window->Clear(GL_DEPTH_BUFFER_BIT);

		const mat4 projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 7.5f);

		while (!m_renders.empty())
		{
			// TODO: work out multiple shadow maps
			m_renders.front()(projection, glm::inverse(lights[0]->transform)); 
			m_renders.pop();
		}

		FinishRecording();
	}

	void Shadows::FinishRecording()
	{
		if (!m_bound)
		{
			return;
		}

		m_bound = false;
		m_shadowMap->Unbind();
	}
}
