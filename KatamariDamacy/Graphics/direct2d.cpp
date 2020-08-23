#include "direct2d.h"
#include "../error_logger.h"

bool Direct2D::Initialize(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view)
{
    HRESULT hr;
	// create d2d factory
	hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
		m_d2d_factory.GetAddressOf());
    if(FAILED(hr))
    {
        ErrorLogger::Log(hr, "Failed to create direct2D factory");
        return false;
    }
    // Add D2D to the swap chain
    Microsoft::WRL::ComPtr <ID3D11Resource> res;
    render_target_view->GetResource(res.GetAddressOf());

    Microsoft::WRL::ComPtr <IDXGISurface> surface;
    res->QueryInterface(__uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf()));

    //auto dpi = static_cast<float>(GetDpiForWindow(hwnd));
	
    auto props =
        D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN,
                D2D1_ALPHA_MODE_PREMULTIPLIED)
        );

    hr = m_d2d_factory->CreateDxgiSurfaceRenderTarget(
        surface.Get(),
        &props,
        m_d2d_render_target.GetAddressOf()
    );
    if (FAILED(hr))
    {
        ErrorLogger::Log(hr, "Failed to create direct2D dxgi render surface");
        return false;
    }





	
    return true;
}
