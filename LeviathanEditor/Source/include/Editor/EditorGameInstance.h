#pragma once

#include <Gameplay/GameInstance.h>
#include <Utility/ResourceStack.h>

#include <glm/mat4x4.hpp>

using glm::mat4;

namespace Leviathan
{
	class ResourceStack;
	class Mesh;
    class Material;
    class OrbitCamera;
    class Shader;
    class Texture;

    class EditorGameInstance : public GameInstance
    {
    private:
        Shader* m_litShader;
        Shader* m_unlitShader;

        Mesh* m_lightMesh;
        Material* m_lightMaterial;
        mat4 m_lightTransform;

        Mesh* m_shaderBallMesh;
        Material* m_shaderBallMaterial;
        mat4 m_shaderBallTransform;

        OrbitCamera* m_orbitCamera;

        Texture* m_baseColorTexture;
        Texture* m_normalTexture;
        Texture* m_ormTexture;

        ResourceStack* m_resourceStack;

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