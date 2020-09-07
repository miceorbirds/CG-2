#include "graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->m_window_width = width;
	this->m_window_height = height;
	this->m_fps_timer.Start();
	if(!InitializeDirectX(hwnd))
		return false;
	if(!m_hud.Initialize(m_render_target_view))
		return false;
	if(!InitializeShaders())
		return false;
	if (!InitializeScene())
		return false;

	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->m_device.Get(), this->m_device_context.Get());
	ImGui::StyleColorsLight();
	
	return true;
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	try
	{
		auto adapters = AdapterReader::GetAdapters();
		if(adapters.empty())
		{
			ErrorLogger::Log("Do DXGI Adapters found");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC swapchain_desc = {0};
		swapchain_desc.BufferDesc.Width = this->m_window_width;
		swapchain_desc.BufferDesc.Height = this->m_window_height;
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
		COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
		hr = this->m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(back_buffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");

		hr = this->m_device->CreateRenderTargetView(back_buffer.Get(), nullptr, this->m_render_target_view.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

		//Describe our Depth/Stencil Buffer
		CD3D11_TEXTURE2D_DESC depth_stencil_texture_desc(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			this->m_window_width, this->m_window_height);
		depth_stencil_texture_desc.MipLevels = 1;
		depth_stencil_texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = this->m_device->CreateTexture2D(&depth_stencil_texture_desc, NULL, this->m_depth_stencil_buffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

		hr = this->m_device->CreateDepthStencilView(this->m_depth_stencil_buffer.Get(), NULL, this->m_depth_stencil_view.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");

		this->m_device_context->OMSetRenderTargets(1, this->m_render_target_view.GetAddressOf(), this->m_depth_stencil_view.Get());

		// Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depth_stencil_desc(D3D11_DEFAULT);
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		hr = this->m_device->CreateDepthStencilState(&depth_stencil_desc, this->m_depth_stencil_state.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		// Create & set the Viewport
		CD3D11_VIEWPORT viewport(.0f,.0f,this->m_window_width,this->m_window_height);
		this->m_device_context->RSSetViewports(1, &viewport);
	
		//Create Rasterizer State (Default)
		CD3D11_RASTERIZER_DESC rasterizer_desc(D3D11_DEFAULT);
		hr = this->m_device->CreateRasterizerState(&rasterizer_desc, this->m_rasterizer_state.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");
		//Create Rasterizer State for culling front
		CD3D11_RASTERIZER_DESC rasterizer_desc_cull_front(D3D11_DEFAULT);
		rasterizer_desc_cull_front.CullMode = D3D11_CULL_FRONT;
		hr = this->m_device->CreateRasterizerState(&rasterizer_desc_cull_front, this->m_rasterizer_state_cull_front.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");
		
		// Create Blend State
		D3D11_RENDER_TARGET_BLEND_DESC render_target_blend_desc {0};
		render_target_blend_desc.BlendEnable = true;
		render_target_blend_desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		render_target_blend_desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		render_target_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
		render_target_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
		render_target_blend_desc.DestBlendAlpha = D3D11_BLEND_ZERO;
		render_target_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		render_target_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		D3D11_BLEND_DESC blend_desc {0};
		blend_desc.RenderTarget[0] = render_target_blend_desc;

		hr = this->m_device->CreateBlendState(&blend_desc, this->m_blend_state.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		//Create sampler description for sampler state
		CD3D11_SAMPLER_DESC sampler_desc(D3D11_DEFAULT);
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = this->m_device->CreateSamplerState(&sampler_desc, this->m_sampler_state.GetAddressOf()); //Create sampler state
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
	}
	catch (COMException &exception)
	{
		ErrorLogger::Log(exception);
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
	this->m_device_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->m_device_context->RSSetState(this->m_rasterizer_state.Get());
	this->m_device_context->OMSetDepthStencilState(this->m_depth_stencil_state.Get(), 0);
	this->m_device_context->OMSetBlendState(this->m_blend_state.Get(),NULL,0xFFF);
	this->m_device_context->PSSetSamplers(0, 1, this->m_sampler_state.GetAddressOf());
	this->m_device_context->VSSetShader(m_vertexshader.GetShader(), NULL, 0);
	this->m_device_context->PSSetShader(m_pixelshader.GetShader(), NULL, 0);

	UINT offset = 0;
	
	static auto alpha = 0.5f;
	{ //Pavement cube
		//Update Constant Buffer
		static float translationOffset[3] = { 0, 0, -1.0f };
		auto world = XMMatrixTranslation(translationOffset[0], translationOffset[1], translationOffset[2]);
		m_cb_vs_vertexshader.data.mat = world * m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix();
		m_cb_vs_vertexshader.data.mat = XMMatrixTranspose(m_cb_vs_vertexshader.data.mat);

		if (!m_cb_vs_vertexshader.ApplyChanges())
			return;
		this->m_device_context->VSSetConstantBuffers(0, 1, this->m_cb_vs_vertexshader.GetAddressOf());

		this->m_cb_ps_pixelshader.data.alpha = alpha;
		this->m_cb_ps_pixelshader.ApplyChanges();
		this->m_device_context->PSSetConstantBuffers(0, 1, this->m_cb_ps_pixelshader.GetAddressOf());

		//Square
		this->m_device_context->PSSetShaderResources(0, 1, this->m_pavement_texture.GetAddressOf());
		this->m_device_context->IASetVertexBuffers(0, 1, m_vertex_buffer.GetAddressOf(), m_vertex_buffer.StridePtr(), &offset);
		this->m_device_context->IASetIndexBuffer(m_index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		this->m_device_context->RSSetState(this->m_rasterizer_state_cull_front.Get());
		this->m_device_context->DrawIndexed(m_index_buffer.BufferSize(), 0, 0);
		this->m_device_context->RSSetState(this->m_rasterizer_state.Get());

		this->m_device_context->DrawIndexed(m_index_buffer.BufferSize(), 0, 0);
	}
	
	
	
	// direct2d
	static auto fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (m_fps_timer.GetMilisecondsElapsed() > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		m_fps_timer.Restart();
	}
	m_hud.RenderText(fpsString);
	m_hud.Draw();


	static int counter = 0;
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//Create ImGui Test Window
	ImGui::Begin("Test");
	ImGui::DragFloat("Alpha", &alpha, 0.1f, 0.0f, 1.0f);
	ImGui::End();
	//Assemble Together Draw Data
	ImGui::Render();
	//Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	this->m_swapchain->Present(1, NULL);
}

bool Graphics::InitializeShaders()
{
	std::wstring shaderfolder;
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
	try
	{
		// Textured Square
		Vertex v[] =
		{
			Vertex(-0.5f,  -0.5f, -0.5f, 0.0f, 1.0f), //FRONT Bottom Left   - [0]
			Vertex(-0.5f,   0.5f, -0.5f, 0.0f, 0.0f), //FRONT Top Left      - [1]
			Vertex(0.5f,   0.5f, -0.5f, 1.0f, 0.0f), //FRONT Top Right     - [2]
			Vertex(0.5f,  -0.5f, -0.5f, 1.0f, 1.0f), //FRONT Bottom Right   - [3]
			Vertex(-0.5f,  -0.5f, 0.5f, 0.0f, 1.0f), //BACK Bottom Left   - [4]
			Vertex(-0.5f,   0.5f, 0.5f, 0.0f, 0.0f), //BACK Top Left      - [5]
			Vertex(0.5f,   0.5f, 0.5f, 1.0f, 0.0f), //BACK Top Right     - [6]
			Vertex(0.5f,  -0.5f, 0.5f, 1.0f, 1.0f), //BACK Bottom Right   - [7]
		};
		// Load vertex data
		auto hr = this->m_vertex_buffer.Initialize(this->m_device.Get(), v, ARRAYSIZE(v));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer.");

		DWORD indices[] =
		{
			0, 1, 2, //FRONT
			0, 2, 3, //FRONT
			4, 7, 6, //BACK 
			4, 6, 5, //BACK
			3, 2, 6, //RIGHT SIDE
			3, 6, 7, //RIGHT SIDE
			4, 5, 1, //LEFT SIDE
			4, 1, 0, //LEFT SIDE
			1, 5, 6, //TOP
			1, 6, 2, //TOP
			0, 3, 7, //BOTTOM
			0, 7, 4, //BOTTOM
		};
		// Load index data
		hr = this->m_index_buffer.Initialize(m_device.Get(),indices,ARRAYSIZE(indices));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer.");

		// Load textures
		hr = CreateWICTextureFromFile(this->m_device.Get(), L"Data\\Textures\\seamless_grass.jpg", nullptr, m_grass_texture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");
		hr = CreateWICTextureFromFile(this->m_device.Get(), L"Data\\Textures\\pinksquare.jpg", nullptr, m_pink_texture.GetAddressOf());
		if (FAILED(hr))
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");
		hr = CreateWICTextureFromFile(this->m_device.Get(), L"Data\\Textures\\seamless_pavement.jpg", nullptr, m_pavement_texture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		//Initialize Constant Buffer(s)
		hr = this->m_cb_vs_vertexshader.Initialize(m_device.Get(), m_device_context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");
		hr = this->m_cb_ps_pixelshader.Initialize(m_device.Get(), m_device_context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		m_camera.SetPosition(0.0f, 0.0f, -2.0f);
		m_camera.SetProjectionValues(90.f, static_cast<float>(m_window_width)/static_cast<float>(m_window_height),0.1f, 1000.f);
	}
	catch(COMException &exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

Graphics::~Graphics()
{
	if (nullptr != ImGui::GetCurrentContext())
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}