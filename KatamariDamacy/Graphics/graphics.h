#pragma once

#include "adapter_reader.h"
#include "shaders.h"
#include "vertex.h"

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

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
};