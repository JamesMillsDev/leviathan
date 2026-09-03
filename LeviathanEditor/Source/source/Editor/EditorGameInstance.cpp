#include "Editor/EditorGameInstance.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Core/Application.h"
#include "Core/Input.h"
#include "Gameplay/ECS/Components/GraphicsComponents.h"
#include "Gameplay/ECS/Components/TransformComponent.h"
#include "Gameplay/ECS/Systems/RenderSystem.h"
#include "Graphics/Light.h"
#include "Graphics/Lighting.h"
#include "Graphics/Renderer.h"
#include "Graphics/Cameras/OrbitCamera.h"
#include "Graphics/Resources/Material.h"
#include "Graphics/Resources/Mesh.h"
#include "Graphics/Resources/ResourceManager.h"
#include "Graphics/Resources/Shader.h"
#include "Graphics/Resources/Textures/Texture.h"

using glm::vec3;

#define TRY_SWAP_DEBUG_MODE(Key, Num) \
	if(input->IsKeyDown(EInputCodes::Key)) \
	{ \
		debugMode = Num;\
	}

namespace Leviathan
{
	EditorGameInstance::EditorGameInstance() :
		m_orbitCamera{ nullptr }, m_resourceStack{ new ResourceStack }, m_light{ nullptr }
	{}

	void EditorGameInstance::Init()
	{
		InitAndPush(
			[this]
			{
				m_ecsManager->RegisterComponent<RenderComponent>();
				m_ecsManager->RegisterComponent<TransformComponent>();

				m_ecsManager->RegisterSystem<RenderSystem>();
			},
			[this]
			{}
		);

		// Camera
		InitAndPush(
			[this]
			{
				m_orbitCamera = new OrbitCamera;
				m_renderer->SetActiveCamera(m_orbitCamera);
			},
			[this]
			{
				delete m_orbitCamera;
			}
		);

		// Light
		InitAndPush(
			[this]
			{
				m_light = new Light;
				m_light->transform = glm::lookAt(vec3{ -1.2f, -1.f, -2.f }, vec3{ 0.f, -.02f, 0.f },
					vec3{ 0.f, 1.f, 0.f });
				m_renderer->AddLight(m_light);
			},
			[this]
			{
				m_renderer->RemoveLight(m_light);
				delete m_light;
			}
		);

		weak_ptr<ResourceManager> resourceManagerPtr = Application::Instance()->GetResourceManager();

		if (auto resourceManager = resourceManagerPtr.lock())
		{
			resourceManager->Register("litShader", new Shader{ "Shaders/deferred_lit" });

			resourceManager->Register("rebarBaseColor", new Texture{ "Content/Textures/T_RebarConcrete_BC.tga", true });
			resourceManager->Register("rebarNormal", new Texture{ "Content/Textures/T_RebarConcrete_N.tga" });
			resourceManager->Register("rebarOrm", new Texture{ "Content/Textures/T_RebarConcrete_ORM.tga" });

			resourceManager->Register("floorBaseColor", new Texture{ "Content/Textures/T_WoodFloor_BC.png", true });
			resourceManager->Register("floorNormal", new Texture{ "Content/Textures/T_WoodFloor_N.png" });
			resourceManager->Register("floorOrm", new Texture{ "Content/Textures/T_WoodFloor_ORM.png" });

			resourceManager->Register("shaderBallMesh", Mesh::MakeFromAssimp("Meshes/shaderBall.fbx"));
			resourceManager->Register("floorMesh", Mesh::MakePlane());

			Material* shaderBallMaterial = new Material{ resourceManager->Find<Shader>("litShader") };
			Material* floorMaterial = new Material{ resourceManager->Find<Shader>("litShader") };
			resourceManager->Register("shaderBallMaterial", shaderBallMaterial);
			resourceManager->Register("floorMaterial", floorMaterial);

			mat4 shaderBallTransform = glm::scale(mat4{ 1.f }, vec3{ .5f });
			shaderBallTransform = glm::translate(shaderBallTransform, { 0.f, -.02f, 0.f });

			mat4 floorTransform = glm::scale(mat4{ 1.f }, vec3{ 4.f });

			shaderBallMaterial->SetMaterialProperty(
				"material.tint", EMaterialPropertyType::Vec3, {
					.v3Value = vec3{1.f}
				}
			);

			shaderBallMaterial->SetMaterialProperty(
				"material.specularStrength",
				EMaterialPropertyType::Float, { .fValue = 0.f }
			);

			shaderBallMaterial->SetTexture("material.baseColor", resourceManager->Find<Texture>("rebarBaseColor"));
			shaderBallMaterial->SetTexture("material.normalMap", resourceManager->Find<Texture>("rebarNormal"));
			shaderBallMaterial->SetTexture("material.ormMap", resourceManager->Find<Texture>("rebarOrm"));

			floorMaterial->SetMaterialProperty(
				"material.tint", EMaterialPropertyType::Vec3, {
					.v3Value = vec3{1.f}
				}
			);

			floorMaterial->SetMaterialProperty(
				"material.specularStrength", EMaterialPropertyType::Float,
				{ .fValue = .25f }
			);

			floorMaterial->SetTexture("material.baseColor", resourceManager->Find<Texture>("floorBaseColor"));
			floorMaterial->SetTexture("material.normalMap", resourceManager->Find<Texture>("floorNormal"));
			floorMaterial->SetTexture("material.ormMap", resourceManager->Find<Texture>("floorOrm"));

			Entity entity = m_ecsManager->MakeEntity();
			m_ecsManager->BuildEntity(entity,
				RenderComponent{
				   .mesh = resourceManager->Find<Mesh>("shaderBallMesh"),
				   .material = shaderBallMaterial
				},
				TransformComponent{
					.transform = shaderBallTransform
				}
			);

			entity = m_ecsManager->MakeEntity();
			m_ecsManager->BuildEntity(entity, 
				RenderComponent{
				   .mesh = resourceManager->Find<Mesh>("floorMesh"),
				   .material = floorMaterial
				},
				TransformComponent{
					.transform = floorTransform
				}
			);
		}
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
		else
		{
			m_orbitCamera->Rotate({ 2.f, 0.f });
		}

		TRY_SWAP_DEBUG_MODE(KeyF6, 0)
			TRY_SWAP_DEBUG_MODE(KeyF1, 1)
			TRY_SWAP_DEBUG_MODE(KeyF2, 2)
			TRY_SWAP_DEBUG_MODE(KeyF3, 3)
			TRY_SWAP_DEBUG_MODE(KeyF4, 4)
			TRY_SWAP_DEBUG_MODE(KeyF5, 5)
	}

	void EditorGameInstance::Render()
	{
		/*m_renderer->Render(m_floorMaterial, m_floorMesh, m_floorTransform);
		m_renderer->Render(m_shaderBallMaterial, m_shaderBallMesh, m_shaderBallTransform);*/
	}

	void EditorGameInstance::Shutdown()
	{
		delete m_resourceStack;
	}

	void EditorGameInstance::InitAndPush(const CleanupFunction& init, const CleanupFunction& cleanup) const
	{
		init();
		m_resourceStack->Push(cleanup);
	}
}
