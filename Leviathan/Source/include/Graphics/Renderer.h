#pragma once

#include <functional>
#include <memory>
#include <queue>
#include <glm/mat4x4.hpp>

#include "Utility/Collections/TList.h"

using std::function;
using std::shared_ptr;
using std::queue;
using glm::mat4;
using glm::vec3;

using CameraTransformGetter = function<void(mat4&, mat4&, vec3&)>;

namespace Leviathan
{
	class Config;
	class Camera;
	class FrameBuffer;
	class Light;
	class Mesh;
	class Material;
	class Window;

	using LightMatrixGetter = function<void(Light*, mat4&, mat4&)>;

	class RenderPass
	{
		friend class Renderer;

	public:
		vec3 viewLocation;
		mat4 viewMatrix;
		mat4 projectionMatrix;
		FrameBuffer* frameBuffer;

		bool useCameraValues;
		uint32 clearMask;

		int32 cullFaceMode;
		bool useFaceCulling;

	private:
		queue<function<void()>> m_renderQueue;
		CameraTransformGetter m_cameraTransformGetter;
		LightMatrixGetter m_lightMatrixGetter;
		function<void()> m_preRender;

	public:
		RenderPass();
		explicit RenderPass(const mat4& prjMatrix);
		explicit RenderPass(FrameBuffer* fb, const mat4& prjMatrix);
		~RenderPass();

	public:
		void QueueRender(const function<void()>& fnc);
		void SetCameraSettingsGetter(const CameraTransformGetter& getter);
		void SetLightMatrixGetter(const LightMatrixGetter& getter);
		void SetPreRenderFnc(const function<void()>& preRenderFnc);

	private:
		void Render(const shared_ptr<Window>& window);

	};

	class Renderer
	{
		friend class Application;

	private:
		Camera* m_camera;

		shared_ptr<Window> m_window;
		shared_ptr<Config> m_config;
		FrameBuffer* m_shadowMap;
		FrameBuffer* m_depthBuffer;

		RenderPass* m_mainRenderPass;
		TList<RenderPass*> m_renderPasses;
		TList<Light*> m_lights;

	private:
		struct PrivateKey
		{};

	public:
		explicit Renderer(PrivateKey);

	public:
		void SetActiveCamera(Camera* camera);
		void AddLight(Light* light);
		void RemoveLight(Light* light);

		void AddRenderPass(RenderPass* pass);
		void InsertRenderPass(RenderPass* pass, int64 index);
		void RemoveRenderPass(RenderPass* pass);

		void Render(Material* material, Mesh* mesh, const mat4& transform);

		[[nodiscard]] FrameBuffer* GetShadowMap() const;
		[[nodiscard]] RenderPass* GetMainRenderPass() const;

	private:
		void Init(const shared_ptr<Window>& window);
		void Render();
		void Shutdown() const;

	};
}