#pragma once

#include <Gameplay/GameInstance.h>
#include <Utility/ResourceStack.h>

#include <glm/mat4x4.hpp>

using glm::mat4;

namespace Leviathan
{
	class RenderPass;
	class ResourceStack;
	class Mesh;
    class Light;
    class Material;
    class OrbitCamera;
    class Shader;
    class Texture;

    class EditorGameInstance : public GameInstance
    {
    private:
        Shader* m_litShader;
        Shader* m_unlitShader;

        Mesh* m_shaderBallMesh;
        Material* m_shaderBallMaterial;
        mat4 m_shaderBallTransform;

        Mesh* m_floorMesh;
        Material* m_floorMaterial;
        mat4 m_floorTransform;

        OrbitCamera* m_orbitCamera;

        Texture* m_rebarBaseColor;
        Texture* m_rebarNormal;
        Texture* m_rebarOrm;

        Texture* m_woodFloorBaseColor;
        Texture* m_woodFloorNormal;

        ResourceStack* m_resourceStack;

        Light* m_light;

    public:
        EditorGameInstance();

    public:
        void Init() override;
        void Tick() override;
        void Render() override;
        void Shutdown() override;

    private:
        void InitAndPush(const CleanupFunction& init, const CleanupFunction& cleanup) const;

    };
}