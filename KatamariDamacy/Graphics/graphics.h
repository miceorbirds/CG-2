#pragma once

#include "adapter_reader.h"
#include "shaders.h"
#include "direct2d.h"
#include <WICTextureLoader.h>
#include "camera.h"
#include <DirectXColors.h>
#include "../timer.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "../Game/renderable_game_object.h"
#include "light.h"
#include "../Game/landscape.h"
#include "shadow_map.h"
#include "deferred_rendertarget.h"
#include "../Game/katamari_thing.h"
#include "pointlight.h"

constexpr int g_numItems = 2;
constexpr int g_numLights = 100;

class Graphics
{
public:
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	~Graphics();

	Camera m_camera;
	RenderableGameObject m_game_object;
	KatamariThing m_katamary;
	std::vector<KatamariThing> m_items;

	Light m_sun;
	RenderableGameObject m_bulb;
	PointLightMesh m_pointlight;
	std::vector<PointLightMesh> m_pointlights;
	Landscape m_land;
private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();

	void UpdateConstantBuffers();
	void RenderToTexture();
	void RenderToGbuff();
	void RenderToWindow();
	void DoDirLightPass();
	void DoPointLightPass();
	void CheckCollision();

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_device_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11Debug> m_debugger;
	CD3D11_VIEWPORT m_viewport;

	ShadowMap* m_shadow_map = nullptr;

	GBufferRT* m_gbuffer = nullptr;

	VertexShader m_vertexshader;
	PixelShader m_pixelshader;

	VertexShader m_depth_vertexshader;
	PixelShader m_depth_pixelshader;

	VertexShader m_deferred_vertexshader;
	PixelShader m_deferred_pixelshader;

	VertexShader m_vertexshader_dirlight_pass;
	PixelShader m_pixelshader_dirlight_pass;

	VertexShader m_vertexshader_pointlight_pass;
	PixelShader m_pixelshader_pointlight_pass;

	ConstantBuffer<CB_VS_vertexshader> m_cb_vs_vertexshader;
	ConstantBuffer<CB_VS_camLight_matrix> m_cb_vs_camlightmatrix;

	ConstantBuffer <CB_PS_light> m_cb_ps_light;
	ConstantBuffer <CB_PS_light_pointbulb> m_cb_ps_pointlight;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depth_stencil_texture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depth_stencil_state;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_depth_disabled_stencil_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizer_state;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blend_state;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler_state_main;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler_state_shadowmap;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler_state_point;

	Direct2D m_hud;
	Timer m_fps_timer;
	int score = 0;

	int m_window_width{ 0 };
	int m_window_height{ 0 };

	std::vector<std::string> pathToItems =
	{
		"Data/Objects/apple.fbx",
		"Data/Objects/broccoli.fbx",
		"Data/Objects/carrot.fbx",
		"Data/Objects/lemon.fbx",
		"Data/Objects/pomgranate.fbx",
		"Data/Objects/strawberry.fbx",
	};
	std::vector<std::string> pathToTextures =
	{
		"Data/Objects/apple03_lr_albedo.jpeg",
		"Data/Objects/49_broccoli_albedo.jpeg",
		"Data/Objects/07_carrot_albedo.jpeg",
		"Data/Objects/11_lemon_albedo.jpeg",
		"Data/Objects/23_pomegranate_albedo.jpeg",
		"Data/Objects/73_strawberry_albedo.jpeg",
	};
};