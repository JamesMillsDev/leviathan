#include "Editor/EditorGameInstance.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Core/Input.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Cameras/OrbitCamera.h"

using glm::vec3;

namespace Leviathan
{
	EditorGameInstance::EditorGameInstance() :
		m_litShader{ nullptr }, m_unlitShader{ nullptr }, m_lightMesh{ nullptr },
		m_lightMaterial{ nullptr }, m_shaderBallMesh{ nullptr }, m_shaderBallMaterial{ nullptr }, m_floorMesh{ nullptr },
		m_floorMaterial{ nullptr }, m_orbitCamera{ new OrbitCamera }, m_rebarBaseColor{ nullptr }, m_rebarNormal{ nullptr },
		m_rebarOrm{ nullptr }, m_woodFloorBaseColor{ nullptr }, m_woodFloorNormal{ nullptr },
		m_resourceStack{ new ResourceStack }
	{
		m_shaderBallTransform = glm::scale(mat4{ 1.f }, vec3{ .5f });

		m_lightTransform = glm::translate(mat4{ 1.f }, vec3{ 1.2f, 1.f, 2.f });
		m_lightTransform = glm::scale(m_lightTransform, vec3{ .2f });

		m_floorTransform = glm::scale(mat4{ 1.f }, vec3{ 4.f });
	}

	void EditorGameInstance::Init()
	{
		// Shaders
		InitAndPush(
			[this]
			{
				m_litShader = new Shader{ "Shaders/lit" };
				m_unlitShader = new Shader{ "Shaders/unlit" };
			},
			[this]
			{
				delete m_unlitShader;
				delete m_litShader;
			}
		);
		// Textures
		InitAndPush(
			[this]
			{
				m_rebarBaseColor = new Texture{ "Content/Textures/T_RebarConcrete_BC.tga" };
				m_rebarNormal = new Texture{ "Content/Textures/T_RebarConcrete_N.tga" };
				m_rebarOrm = new Texture{ "Content/Textures/T_RebarConcrete_ORM.tga" };

				m_woodFloorBaseColor = new Texture{ "Content/Textures/T_WoodFloor_BC.png" };
				m_woodFloorNormal = new Texture{ "Content/Textures/T_WoodFloor_N.png" };
			},
			[this]
			{
				delete m_woodFloorNormal;
				delete m_woodFloorBaseColor;

				delete m_rebarOrm;
				delete m_rebarNormal;
				delete m_rebarBaseColor;
			}
		);
		// Light cube
		InitAndPush(
			[this]
			{
				m_lightMesh = Mesh::MakeCube();

				m_lightMaterial = new Material{ m_unlitShader };
				m_lightMaterial->SetMaterialProperty("material.tint", EMaterialPropertyType::Vec3, { .v3Value = vec3{ 1.f } });
			},
			[this]
			{
				delete m_lightMaterial;
				delete m_lightMesh;
			}
		);
		// Shader ball
		InitAndPush(
			[this]
			{
				m_shaderBallMesh = Mesh::MakeFromAssimp("Meshes/shaderBall.fbx");

				m_shaderBallMaterial = new Material{ m_litShader };

				m_shaderBallMaterial->SetMaterialProperty(
					"material.tint", EMaterialPropertyType::Vec3, { .v3Value = vec3{ 1.f } }
				);

				m_shaderBallMaterial->SetMaterialProperty(
					"material.specularStrength", EMaterialPropertyType::Float, { .fValue = 0.f }
				);

				m_shaderBallMaterial->SetTexture("material.baseColor", m_rebarBaseColor);
				m_shaderBallMaterial->SetTexture("material.normalMap", m_rebarNormal);
				m_shaderBallMaterial->SetTexture("material.ormMap", m_rebarOrm);
			},
			[this]
			{
				delete m_shaderBallMaterial;
				delete m_shaderBallMesh;
			}
		);
		// Floor
		InitAndPush(
			[this]
			{
				m_floorMesh = Mesh::MakePlane();

				m_floorMaterial = new Material{ m_litShader };

				m_floorMaterial->SetMaterialProperty(
					"material.tint", EMaterialPropertyType::Vec3, { .v3Value = vec3{ 1.f } }
				);

				m_floorMaterial->SetMaterialProperty(
					"material.specularStrength", EMaterialPropertyType::Float, { .fValue = .25f }
				);

				m_floorMaterial->SetTexture("material.baseColor", m_woodFloorBaseColor);
				m_floorMaterial->SetTexture("material.normalMap", m_woodFloorNormal);
			},
			[this]
			{
				delete m_floorMaterial;
				delete m_floorMesh;
			}
		);

		m_renderer->SetActiveCamera(m_orbitCamera);
	}

	void EditorGameInstance::Tick()
	{
		Input* input = Input::GetInstance();

		if (input->WasMouseButtonPressed(EInputCodes::MouseButtonRight))
		{
			glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else if (input->WasMouseButtonReleased(EInputCodes::MouseButtonRight))
		{
			glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		if (input->IsMouseButtonDown(EInputCodes::MouseButtonRight))
		{
			vec2 mouseDelta{};
			input->GetMouseDelta(&mouseDelta.x, &mouseDelta.y);

			m_orbitCamera->Rotate(mouseDelta);
			m_orbitCamera->Zoom(input->GetMouseScroll());
		}
	}

	void EditorGameInstance::Render()
	{
		m_renderer->Render(m_lightMaterial, m_lightMesh, m_lightTransform);
		m_renderer->Render(m_floorMaterial, m_floorMesh, m_floorTransform);
		m_renderer->Render(m_shaderBallMaterial, m_shaderBallMesh, m_shaderBallTransform);
	}

	void EditorGameInstance::Shutdown()
	{
		delete m_resourceStack;
		delete m_orbitCamera;
	}

	void EditorGameInstance::InitAndPush(const CleanupFunction& init, const CleanupFunction& cleanup) const
	{
		init();
		m_resourceStack->Push(cleanup);
	}
}
