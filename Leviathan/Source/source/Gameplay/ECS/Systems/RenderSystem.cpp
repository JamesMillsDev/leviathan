#include "Gameplay/ECS/Systems/RenderSystem.h"

#include "Gameplay/ECS/Components/ComponentManager.h"
#include "Gameplay/ECS/Components/GraphicsComponents.h"
#include "Gameplay/ECS/Components/TransformComponent.h"
#include "Graphics/Renderer.h"

namespace Leviathan
{
	Signature RenderSystem::GetSystemSignature(const shared_ptr<ComponentManager>& componentManager)
	{
		Signature signature;

		signature.set(componentManager->GetComponentType<TransformComponent>());
		signature.set(componentManager->GetComponentType<RenderComponent>());

		return signature;
	}

	void RenderSystem::Render(const shared_ptr<ComponentManager>& componentManager)
	{
		for (const Entity& entity : m_entities)
		{
			const auto [transform] = componentManager->GetComponent<TransformComponent>(entity);
			const auto [mesh, material] = componentManager->GetComponent<RenderComponent>(entity);

			renderer->Render(material, mesh, transform);
		}
	}
}
