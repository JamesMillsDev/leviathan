#pragma once

#include <Gameplay/GameInstance.h>

#include <glm/mat4x4.hpp>

using glm::mat4;

namespace Leviathan
{
    class Mesh;
    class Material;
    class Shader;
    class OrbitCamera;

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

    public:
        EditorGameInstance();

    public:
        void Init() override;
        void Tick() override;
        void Render() override;
        void Shutdown() override;

    };
}