#include "Editor/EditorGameInstance.h"

#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Core/Input.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Cameras/OrbitCamera.h"

using glm::vec3;

namespace Leviathan
{
	EditorGameInstance::EditorGameInstance() :
		m_litShader{ nullptr }, m_unlitShader{ nullptr }, m_lightMesh{ nullptr },
		m_lightMaterial{ nullptr }, m_shaderBallMesh{ nullptr }, m_shaderBallMaterial{ nullptr },
		m_orbitCamera{ new OrbitCamera }
	{
		m_shaderBallTransform = glm::scale(mat4{ 1.f }, vec3{ .5f });

		m_lightTransform = glm::translate(mat4{ 1.f }, vec3{ 1.2f, 1.f, 2.f });
		m_lightTransform = glm::scale(m_lightTransform, vec3{ .2f });
	}

	void EditorGameInstance::Init()
	{
		m_litShader = new Shader{ "Shaders/lit" };
		m_unlitShader = new Shader{ "Shaders/unlit" };

		m_lightMaterial = new Material{ m_unlitShader };
		m_lightMaterial->SetMaterialProperty("material.tint", EMaterialPropertyType::Vec3, { .v3Value = vec3{ 1.f } });
		m_lightMesh = Mesh::MakeCube();

		m_shaderBallMaterial = new Material{ m_litShader };
		m_shaderBallMaterial->SetMaterialProperty("material.tint", EMaterialPropertyType::Vec3, { .v3Value = vec3{ 1.f, .5f, .31f } });
		m_shaderBallMaterial->SetMaterialProperty("material.specularStrength", EMaterialPropertyType::Float, { .fValue = 1.f });
		m_shaderBallMesh = Mesh::MakeFromAssimp("Meshes/shaderBall.fbx");

		m_renderer->SetActiveCamera(m_orbitCamera);
	}

	void EditorGameInstance::Tick()
	{
		Input* input = Input::GetInstance();

		if (input->IsMouseButtonDown(EInputCodes::MouseButtonRight))
		{
			vec2 mouseDelta{};
			input->GetMouseDelta(&mouseDelta.x, &mouseDelta.y);

			m_orbitCamera->Rotate(mouseDelta);
		}
	}

	void EditorGameInstance::Render()
	{
		m_renderer->Render(m_lightMaterial, m_lightMesh, m_lightTransform);
		m_renderer->Render(m_shaderBallMaterial, m_shaderBallMesh, m_shaderBallTransform);
	}

	void EditorGameInstance::Shutdown()
	{
		delete m_litShader;
		delete m_unlitShader;

		delete m_lightMesh;
		delete m_lightMaterial;

		delete m_shaderBallMesh;
		delete m_shaderBallMaterial;

		delete m_orbitCamera;
	}
}
