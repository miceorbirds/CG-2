#pragma once

#include "adapter_reader.h"
#include "shaders.h"
#include "vertex.h"
#include "direct2d.h"

class Graphics
{
public:
    bool Initialize(HWND hwnd, int width, int height);
    void RenderFrame();
private:
    bool InitializeDirectX(HWND hwnd, int width, int height);
    bool InitializeShaders();
    bool InitializeScene();

    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_device_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapchain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_render_target_view;

    VertexShader m_vertexshader;
    PixelShader m_pixelshader;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertex_buffer2;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depth_stencil_view;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depth_stencil_buffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depth_stencil_state;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizer_state;

    Direct2D m_hud;

};
