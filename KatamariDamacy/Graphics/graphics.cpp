#include "graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	if(!InitializeDirectX(hwnd, width,height))
		return false;
	if(!m_hud.Initialize(m_render_target_view))
		return false;
	if(!InitializeShaders())
		return false;
	if (!InitializeScene())
		return false;
	return true;
}

bool Graphics::InitializeDirectX(HWND hwnd, int width, int height)
{
	auto adapters = AdapterReader::GetAdapters();
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
			D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT, //FLAGS FOR RUNTIME LAYERS
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

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = this->m_device->CreateTexture2D(&depthStencilDesc, NULL, this->m_depth_stencil_buffer.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil buffer.");
		return false;
	}

	hr = this->m_device->CreateDepthStencilView(this->m_depth_stencil_buffer.Get(), NULL, this->m_depth_stencil_view.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil view.");
		return false;
	}


	this->m_device_context->OMSetRenderTargets(1, this->m_render_target_view.GetAddressOf(), this->m_depth_stencil_view.Get());

	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	hr = this->m_device->CreateDepthStencilState(&depthstencildesc, this->m_depth_stencil_state.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil state.");
		return false;
	}

	// Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// Set the viewport
	this->m_device_context->RSSetViewports(1, &viewport);
	

	//Create Rasterizer State
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	hr = this->m_device->CreateRasterizerState(&rasterizerDesc, this->m_rasterizer_state.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state.");
		return false;
	}

	//Create sampler description for sampler state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = this->m_device->CreateSamplerState(&sampDesc, this->m_sampler_state.GetAddressOf()); //Create sampler state
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create sampler state.");
		return false;
	}
	

	return true;
}

void Graphics::RenderFrame()
{
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	this->m_device_context->ClearRenderTargetView(this->m_render_target_view.Get(), bgcolor);
	this->m_device_context->ClearDepthStencilView(this->m_depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->m_device_context->IASetInputLayout(this->m_vertexshader.GetInputLayout());
	this->m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->m_device_context->RSSetState(this->m_rasterizer_state.Get());
	this->m_device_context->OMSetDepthStencilState(this->m_depth_stencil_state.Get(), 0);
	this->m_device_context->PSSetSamplers(0, 1, this->m_sampler_state.GetAddressOf());
	
	this->m_device_context->VSSetShader(m_vertexshader.GetShader(), NULL, 0);
	this->m_device_context->PSSetShader(m_pixelshader.GetShader(), NULL, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	
	// Square
	this->m_device_context->PSSetShaderResources(0, 1, this->m_texture.GetAddressOf());
	this->m_device_context->IASetVertexBuffers(0, 1, m_vertex_buffer.GetAddressOf(), &stride, &offset);
	this->m_device_context->Draw(6, 0);

	// direct2d
	m_hud.Draw();
	
	this->m_swapchain->Present(1, NULL);
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
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0   },

	};

	UINT numElements = ARRAYSIZE(layout);

	if (!m_vertexshader.Initialize(this->m_device, shaderfolder + L"vertexshader.cso", layout, numElements))
	{
		return false;
	}
	if (!m_pixelshader.Initialize(this->m_device, shaderfolder + L"pixelshader.cso"))
		return false;


	return true;
}

bool Graphics::InitializeScene()
{
	// Textured Square
	Vertex v[] =
	{
		Vertex(-0.5f, -0.5f, 1.0f, 0.0f, 1.0f), //Bottom Left
		Vertex(-0.5f,   0.5f, 1.0f, 0.0f, 0.0f), //Top Left
		Vertex(0.5f,   0.5f, 1.0f, 1.0f, 0.0f), //Top Right

		Vertex(-0.5f, -0.5f, 1.0f, 0.0f, 1.0f), //Bottom Left 
		Vertex(0.5f,   0.5f, 1.0f, 1.0f, 0.0f), //Top Right
		Vertex(0.5f,  -0.5f, 1.0f, 1.0f, 1.0f), //Bottom Right

	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(v);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;

	HRESULT hr = this->m_device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->m_vertex_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	hr = DirectX::CreateWICTextureFromFile(
		this->m_device.Get(),
		L"Data\\Textures\\piano.png",
		nullptr,
		m_texture.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create wic texture from file.");
		return false;
	}
	
	return true;
}