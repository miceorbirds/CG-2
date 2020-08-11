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
            this->swapchain_.GetAddressOf(), //Swapchain Address
            this->device_.GetAddressOf(), //Device Address
            nullptr,  //Supported feature level
            this->device_context_.GetAddressOf() //Device Context Address
            );
    if(FAILED(hr))
    {
        ErrorLogger::Log(hr, "Failed to create device and swapchain");
        return false;
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = this->swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr)) //If error occurred
    {
        ErrorLogger::Log(hr, "GetBuffer Failed.");
        return false;
    }

    hr = this->device_->CreateRenderTargetView(backBuffer.Get(), nullptr, this->render_target_view_.GetAddressOf());
    if (FAILED(hr)) //If error occurred
    {
        ErrorLogger::Log(hr, "Failed to create render target view.");
        return false;
    }

    this->device_context_->OMSetRenderTargets(1, this->render_target_view_.GetAddressOf(), nullptr);
    return true;
}

void Graphics::RenderFrame()
{
    float bgcolor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    this->device_context_->ClearRenderTargetView(this->render_target_view_.Get(), bgcolor);
    this->swapchain_->Present(1, 0);
}

bool Graphics::InitializeShaders()
{
    D3D11_INPUT_ELEMENT_DESC layout[] =
            {
                    {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
            };

    UINT numElements = ARRAYSIZE(layout);

    HRESULT hr = this->device_->CreateInputLayout(layout, numElements, this->vertex_shader_buffer_->GetBufferPointer(), this->vertex_shader_buffer_->GetBufferSize(), this->input_layout_.GetAddressOf());
    if (FAILED(hr))
    {
        ErrorLogger::Log(hr, "Failed to create input layout.");
        return false;
    }
    return true;
}