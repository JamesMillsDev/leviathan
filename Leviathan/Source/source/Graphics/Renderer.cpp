#include "Graphics/Renderer.h"

#include "Graphics/Camera.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"

namespace Leviathan
{
	Renderer::Renderer(PrivateKey)
		: m_camera{ nullptr }
	{

	}

	void Renderer::SetActiveCamera(Camera* camera)
	{
		m_camera = camera;
	}

	void Renderer::Render(Material* material, Mesh* mesh, const mat4& transform) const
	{
		if (m_camera == nullptr)
		{
			return;
		}

		if (!material->Bind())
		{
			return;
		}

		material->Set(material->m_vpLoc, m_camera->ProjectionView());
		material->Set(material->m_cameraLocationLoc, m_camera->Location());

		material->Set(material->m_modelLoc, transform);
		material->Set(material->m_normalMatrixLoc, mat3(glm::transpose(glm::inverse(transform))));

		material->SetMaterialProperties();
		material->Set("lights[0].color", vec3{ 1.f });
		material->Set("lights[0].location", vec3{ 1.2f, 1.0f, 2.0f });

		mesh->Render();
	}
}
