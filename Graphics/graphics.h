//
// Created by mi on 07.08.2020.
//
#pragma once
#ifndef CG_GRAPHICS_H
#define CG_GRAPHICS_H

#include "adapter_reader.h"

class Graphics
{
public:
    bool Initialize(HWND hwnd, int width, int height);
    void RenderFrame();
private:
    bool InitializeDirectX(HWND hwnd, int width, int height);
    bool InitializeShaders();

    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view_;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_;
    Microsoft::WRL::ComPtr<ID3D10Blob> vertex_shader_buffer_;
};


#endif //CG_GRAPHICS_H
