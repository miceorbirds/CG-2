#include "graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
    if(!InitializeDirectX(hwnd, width,height))
        return false;
    if(!InitializeShaders())
        return false;
    return true;
}

bool Graphics::InitializeDirectX(HWND hwnd, int width, int height)
{
    std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
    if(adapters.empty())
    {
        ErrorLogger::Log("Do DXGI Adapters found");
        return false;
    }


    DXGI_SWAP_CHAIN_DESC swapchain_desc;
    ZeroMemory(&swapchain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapchain_desc.BufferDesc.Width = width;
    swapchain_desc.BufferDesc.Height = height;
    swapchain_desc.BufferDesc.RefreshRate.Numerator = 60;
    swapchain_desc.BufferDesc.RefreshRate.Denominator = 1;
    swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapchain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    swapchain_desc.SampleDesc.Count = 1;
    swapchain_desc.SampleDesc.Quality = 0;

    swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desc.BufferCount = 1;
    swapchain_desc.OutputWindow = hwnd;
    swapchain_desc.Windowed = true;
    swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    HRESULT hr;
    hr = D3D11CreateDeviceAndSwapChain(
            adapters[0].pAdapter, //DXGI Adapter
            D3D_DRIVER_TYPE_UNKNOWN,
            nullptr, //FOR SOFTWARE DRIVER TYPE
            D3D11_CREATE_DEVICE_DEBUG, //FLAGS FOR RUNTIME LAYERS
            nullptr, //FEATURE LEVELS ARRAY
            0,  //# OF FEATURE LEVELS IN ARRAY
            D3D11_SDK_VERSION,
            &swapchain_desc, // swapchain description
            this->m_swapchain.GetAddressOf(), //Swapchain Address
            this->m_device.GetAddressOf(), //Device Address
            nullptr,  //Supported feature level
            this->m_device_context.GetAddressOf() //Device Context Address
            );
    if(FAILED(hr))
    {
        ErrorLogger::Log(hr, "Failed to create device and swapchain");
        return false;
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = this->m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr)) //If error occurred
    {
        ErrorLogger::Log(hr, "GetBuffer Failed.");
        return false;
    }

    hr = this->m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, this->m_render_target_view.GetAddressOf());
    if (FAILED(hr)) //If error occurred
    {
        ErrorLogger::Log(hr, "Failed to create render target view.");
        return false;
    }

    this->m_device_context->OMSetRenderTargets(1, this->m_render_target_view.GetAddressOf(), nullptr);
    return true;
}

void Graphics::RenderFrame()
{
    float bgcolor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    this->m_device_context->ClearRenderTargetView(this->m_render_target_view.Get(), bgcolor);
    this->m_swapchain->Present(1, 0);
}

bool Graphics::InitializeShaders()
{
    std::wstring shaderfolder = L"";
#pragma region DetermineShaderPath
    if (IsDebuggerPresent() == TRUE)
    {
#ifdef _DEBUG //Debug Mode
    #ifdef _WIN64 //x64
            shaderfolder = L"..\\x64\\Debug\\";
    #else  //x86 (Win32)
          shaderfolder = L"..\\Debug\\";
    #endif
    #else //Release Mode
    #ifdef _WIN64 //x64
        shaderfolder = L"..\\x64\\Release\\";
    #else  //x86 (Win32)
        shaderfolder = L"..\\Release\\";
    #endif
#endif
    }


    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
    };

    UINT numElements = ARRAYSIZE(layout);

    if (!m_vertexshader.Initialize(this->m_device, shaderfolder + L"vertexshader.cso", layout, numElements))
    {
        return false;
    }


    return true;
}