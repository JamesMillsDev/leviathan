#pragma once

#include <functional>
#include <memory>
#include <queue>
#include <glm/mat4x4.hpp>

#include "Utility/Collections/TList.h"
#include "Utility/EnumHelpers.h"

using std::function;
using std::shared_ptr;
using std::queue;
using glm::mat4;
using glm::vec3;

using CameraTransformGetter = function<void(mat4&, mat4&, vec3&)>;

namespace Leviathan
{
	class Shader;
	class GBuffer;
	class Config;
	class Camera;
	class FrameBuffer;
	class Light;
	class Mesh;
	class Material;
	class Window;

	/**
	 * A set of flags that can determine what components the pass should render
	 */
	enum class EPassFeature : uint8
	{
		None = 0,
		FaceCulling = 1 << 0,
		Lighting = 1 << 1,
		ShadowMapping = 1 << 2,
		OverrideCameraMatrices = 1 << 3,
		WritesShadowMap = 1 << 4,
	};

	ENUM_OPERATORS(EPassFeature, uint8)

	class RenderPass
	{
		friend class Renderer;

		using VoidSignature = function<void()>;
		using RenderFncSignature = function<void(const mat4&, const mat4&, const vec3&)>;
		using CameraMatrixSignature = function<void(mat4&, mat4&, vec3&)>;
		using LightMatrixSignature = function<void(Light*, mat4&, mat4&)>;

	public:
		EPassFeature features;

		int32 cullFaceMode;
		uint32 clearMask;

		FrameBuffer* frameBuffer;

		Shader* passShader;
		Material* passMaterial;

	private:
		queue<RenderFncSignature> m_renderFncs;

		VoidSignature m_preRenderFnc;
		VoidSignature m_postRenderFnc;

		CameraMatrixSignature m_cameraMatrixGetter;
		LightMatrixSignature m_lightMatrixGetter;

	public:
		RenderPass();
		~RenderPass();

	public:
		void SetPreRenderFnc(const VoidSignature& preRender);
		void SetPostRenderFnc(const VoidSignature& postRender);
		void SetLightMatrixGetter(const LightMatrixSignature& getter);

		void EnablePassFeature(EPassFeature feature) const;
		void DisablePassFeature(EPassFeature feature) const;

	private:
		bool IsFeatureEnabled(EPassFeature feature) const;

		void Render(const shared_ptr<Window>& window, const mat4& projection, const mat4& view, const vec3& location);

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

		Mesh* m_screenMesh;

		RenderPass* m_gBufferPass;
		TList<RenderPass*> m_renderPasses;
		TList<Light*> m_lights;

		GBuffer* m_gBuffer;

	private:
		struct PrivateKey
		{};

	public:
		explicit Renderer(PrivateKey);

	public:
		void SetActiveCamera(Camera* camera);
		void AddLight(Light* light);
		void RemoveLight(Light* light);

		void Render(Material* material, Mesh* mesh, const mat4& transform);

		[[nodiscard]] FrameBuffer* GetShadowMap() const;

	private:
		void Init(const shared_ptr<Window>& window);
		void Render();
		void Shutdown() const;

	};
}