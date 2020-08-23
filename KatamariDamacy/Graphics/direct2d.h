#pragma once

#include <d3d11.h>
#include <d2d1_1.h>
#include <wrl/client.h>
#pragma comment(lib, "d2d1.lib")

class Direct2D
{
public:
    bool Initialize(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view);
    void Update();
    void Draw();
private:
    Microsoft::WRL::ComPtr<ID2D1Factory> m_d2d_factory;
    Microsoft::WRL::ComPtr<ID2D1RenderTarget> m_d2d_render_target;



};


