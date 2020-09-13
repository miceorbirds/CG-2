#pragma once

#include "adapter_reader.h"
#include "shaders.h"
#include "direct2d.h"
#include <WICTextureLoader.h>
#include "camera.h"
#include "../timer.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "../Game/renderable_game_object.h"
#include "light.h"

class Graphics
{
public:
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	~Graphics();

	Camera m_camera;
	RenderableGameObject m_game_object;
	Light m_light;
private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_device_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_render_target_view;

	VertexShader m_vertexshader;
	PixelShader m_pixelshader;
	PixelShader m_pixelshader_nolight;

	ConstantBuffer<CB_VS_vertexshader> m_cb_vs_vertexshader;
	//ConstantBuffer <CB_PS_pixelshader> m_cb_ps_pixelshader;
	ConstantBuffer <CB_PS_light> m_cb_ps_light;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depth_stencil_buffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depth_stencil_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizer_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizer_state_cull_front;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blend_state;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler_state;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pink_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_grass_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pavement_texture;

	Direct2D m_hud;
	Timer m_fps_timer;

	int m_window_width{ 0 };
	int m_window_height{ 0 };
};