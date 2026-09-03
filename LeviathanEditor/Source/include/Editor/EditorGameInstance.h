#pragma once

#include <Gameplay/GameInstance.h>
#include <Utility/ResourceStack.h>

namespace Leviathan
{
	class ResourceStack;
    class Light;
    class OrbitCamera;

    class EditorGameInstance : public GameInstance
    {
    private:
        OrbitCamera* m_orbitCamera;
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