#include "graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	srand(static_cast <unsigned> (time(0)));

	this->m_window_width = width;
	this->m_window_height = height;
	this->m_fps_timer.Start();
	if (!InitializeDirectX(hwnd))
		return false;
	if (!m_hud.Initialize(m_render_target_view))
		return false;
	if (!InitializeShaders())
		return false;
	if (!InitializeScene())
		return false;
	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->m_device.Get(), this->m_device_context.Get());
	ImGui::StyleColorsDark();

	return true;
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	try
	{
		auto adapters = AdapterReader::GetAdapters();
		if (adapters.empty())
		{
			ErrorLogger::Log("Do DXGI Adapters found");
			return false;
		}
		DXGI_SWAP_CHAIN_DESC swapchain_desc = { 0 };
		swapchain_desc.BufferDesc.Width = this->m_window_width;
		swapchain_desc.BufferDesc.Height = this->m_window_height;
		swapchain_desc.BufferDesc.RefreshRate.Numerator = 1;
		swapchain_desc.BufferDesc.RefreshRate.Denominator = 60;
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

		/* create DEVICE, DEVICE CONTEXT and SWAPCHAIN */
		auto hr = D3D11CreateDeviceAndSwapChain(
			adapters[0].pAdapter, //DXGI Adapter
			D3D_DRIVER_TYPE_UNKNOWN,
			nullptr,
			D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			nullptr, //feature levels array
			0, //# of feature levels in array
			D3D11_SDK_VERSION,
			&swapchain_desc, // swapchain description
			this->m_swapchain.GetAddressOf(), //Swapchain Address
			this->m_device.GetAddressOf(), //Device Address
			nullptr, //Supported feature level
			this->m_device_context.GetAddressOf() //Device Context Address
		);
		COM_ERROR_IF_FAILED(hr, "Failed to create device, device context and swapchain.");

		// get Backbuffer from swapchain and use it as rendertargetview. backbuffer is where our picture are painted.
		Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
		hr = this->m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
			reinterpret_cast<void**>(back_buffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");

		CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		hr = this->m_device->CreateRenderTargetView(back_buffer.Get(), &renderTargetViewDesc, this->m_render_target_view.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

		//create gbuffer RTs
		m_gbuffer = new GBufferRT;
		m_gbuffer->Init(this->m_device.Get(), m_window_width, m_window_height);

		// get Debugger for future using
		hr = m_device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(m_debugger.GetAddressOf()));

		// DEPTH STENSIL SETTINGS
		//Describe our Depth/Stencil Buffer (== depth stensil texture)
		CD3D11_TEXTURE2D_DESC depth_stencil_texture_desc(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			this->m_window_width, this->m_window_height);
		depth_stencil_texture_desc.MipLevels = 1;
		depth_stencil_texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		// now create resource for depth stensil buffer. no initial data required
		hr = this->m_device->CreateTexture2D(&depth_stencil_texture_desc, nullptr, this->m_depth_stencil_texture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");
		// create depth stensil resource view so we can use depth stensil texture in the pipeline
		hr = this->m_device->CreateDepthStencilView(this->m_depth_stencil_texture.Get(), nullptr, this->m_depth_stencil_view.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");

		this->m_device_context->OMSetRenderTargets(1, this->m_render_target_view.GetAddressOf(), this->m_depth_stencil_view.Get());

		// Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depth_stencil_desc(D3D11_DEFAULT);
		depth_stencil_desc.DepthEnable = true;
		/// depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = this->m_device->CreateDepthStencilState(&depth_stencil_desc, this->m_depth_stencil_state.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");
		// create depthstencil state for 2d rendering
		depth_stencil_desc.DepthEnable = false;
		hr = this->m_device->CreateDepthStencilState(&depth_stencil_desc, this->m_depth_disabled_stencil_state.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		// Create & set the Viewport
		m_viewport.Width = static_cast<float>(this->m_window_width);
		m_viewport.Height = static_cast<float>(this->m_window_height);
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		this->m_device_context->RSSetViewports(1, &m_viewport);

		// create shadowmap things
		m_shadow_map = new ShadowMap();
		if (!m_shadow_map->Initialize(this->m_device.Get(), 0.1f, 10000.0f))
			return false;

		//Create Rasterizer State
		CD3D11_RASTERIZER_DESC rasterizer_desc(D3D11_DEFAULT);
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.DepthClipEnable = false;
		hr = this->m_device->CreateRasterizerState(&rasterizer_desc, this->m_rasterizer_state.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//// Create Blend State
		/*D3D11_RENDER_TARGET_BLEND_DESC render_target_blend_desc{ 0 };
		render_target_blend_desc.BlendEnable = true;
		render_target_blend_desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		render_target_blend_desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		render_target_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
		render_target_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
		render_target_blend_desc.DestBlendAlpha = D3D11_BLEND_ZERO;
		render_target_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		render_target_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		D3D11_BLEND_DESC blend_desc{ 0 };
		blend_desc.RenderTarget[0] = render_target_blend_desc;
		hr = this->m_device->CreateBlendState(&blend_desc, this->m_blend_state.GetAddressOf());*/
		//Blend state setup
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		m_device->CreateBlendState(&blendDesc, this->m_blend_state.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		//Create sampler description for main sampler state
		CD3D11_SAMPLER_DESC sampler_desc(D3D11_DEFAULT);
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.BorderColor[0] = 0.0f;
		sampler_desc.BorderColor[1] = 0.0f;
		sampler_desc.BorderColor[2] = 0.0f;
		sampler_desc.BorderColor[3] = 0.0f;
		sampler_desc.MinLOD = 0;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		//Create main sampler state
		hr = this->m_device->CreateSamplerState(&sampler_desc, this->m_sampler_state_main.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");

		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		hr = this->m_device->CreateSamplerState(&sampler_desc, this->m_sampler_state_shadowmap.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth sampler state.");

		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		hr = this->m_device->CreateSamplerState(&sampler_desc, this->m_sampler_state_point.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create point sampler state.");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void Graphics::RenderFrame()
{
	CheckCollision();
	this->m_sun.UpdateViewMatrix(this->m_camera.GetPositionFloat3());

	UpdateConstantBuffers();
	this->m_device_context->RSSetState(nullptr);
	this->m_device_context->IASetInputLayout(this->m_vertexshader.GetInputLayout());
	this->m_device_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->m_device_context->OMSetDepthStencilState(this->m_depth_stencil_state.Get(), 0);
	this->m_device_context->OMSetBlendState(nullptr, nullptr, 0xFFF);

	this->m_device_context->PSSetSamplers(0, 1, this->m_sampler_state_main.GetAddressOf());
	RenderToTexture();
	RenderToGbuff();
	this->m_device_context->PSSetSamplers(0, 1, this->m_sampler_state_point.GetAddressOf());
	this->m_device_context->PSSetSamplers(1, 1, this->m_sampler_state_shadowmap.GetAddressOf());
	RenderToWindow();

	// direct2d
	static auto fps_counter = 0;
	static std::string fps_string = "FPS: 0";
	fps_counter += 1;
	if (m_fps_timer.GetMilisecondsElapsed() > 1000.0)
	{
		fps_string = "FPS: " + std::to_string(fps_counter);
		fps_counter = 0;
		m_fps_timer.Restart();
	}
	m_hud.RenderText(fps_string);
	m_hud.Draw();

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//Create ImGui Test Window
	ImGui::Begin("Light Controls");
	ImGui::DragFloat3("Ambient Light Color", &this->m_sun.ambient_light_color.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient Light Strength", &this->m_sun.ambient_light_strength, 0.01f, 0.0f, 1.0f);
	ImGui::NewLine();
	ImGui::DragFloat3("Diffuse Light Color", &this->m_sun.diffuse_light_color.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Diffuse Light Strength", &this->m_sun.diffuse_light_strength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Specular Strength", &this->m_sun.specular_strength, 0.01f, 0.0f, 10.0f);
	ImGui::End();

	/*ImGui::Begin("Albedo");
	ImGui::Image(m_gbuffer->m_shaderResourceViewArray[0], ImVec2(400, 400));
	ImGui::End();

	ImGui::Begin("Normal");
	ImGui::Image(m_gbuffer->m_shaderResourceViewArray[1], ImVec2(400, 400));
	ImGui::End();

	ImGui::Begin("Position");
	ImGui::Image(m_gbuffer->m_shaderResourceViewArray[2], ImVec2(400, 400));
	ImGui::End();*/

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
#else //x86 (Win32)
		shaderfolder = L"..\\Debug\\";
#endif
#else //Release Mode
#ifdef _WIN64 //x64
		shaderfolder = L"..\\x64\\Release\\";
#else //x86 (Win32)
		shaderfolder = L"..\\Release\\";
#endif
#endif
	}

	D3D11_INPUT_ELEMENT_DESC layout_description[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	UINT num_elements = ARRAYSIZE(layout_description);
	if (!m_vertexshader.Initialize(this->m_device, shaderfolder + L"vertexshader_shadow.cso", layout_description, num_elements))
		return false;
	if (!m_pixelshader.Initialize(this->m_device, shaderfolder + L"pixelshader_shadow.cso"))
		return false;

	if (!m_deferred_vertexshader.Initialize(this->m_device, shaderfolder + L"vertexshader_deferred.cso", layout_description, num_elements))
		return false;
	if (!m_deferred_pixelshader.Initialize(this->m_device, shaderfolder + L"pixelshader_deferred.cso"))
		return false;

	if (!m_depth_vertexshader.Initialize(this->m_device, shaderfolder + L"vertexshader_depth.cso", layout_description, num_elements))
		return false;
	if (!m_depth_pixelshader.Initialize(this->m_device, shaderfolder + L"pixelshader_depth.cso"))
		return false;

	if (!m_vertexshader_dirlight_pass.Initialize(this->m_device, shaderfolder + L"vertexshader_dirlight_pass.cso", layout_description, num_elements))
		return false;
	if (!m_pixelshader_dirlight_pass.Initialize(this->m_device, shaderfolder + L"pixelshader_dirlight_pass.cso"))
		return false;

	if (!m_vertexshader_pointlight_pass.Initialize(this->m_device, shaderfolder + L"vertexshader_pointlight_pass.cso", layout_description, num_elements))
		return false;
	if (!m_pixelshader_pointlight_pass.Initialize(this->m_device, shaderfolder + L"pixelshader_pointlight_pass.cso"))
		return false;

	return true;
}

bool Graphics::InitializeScene()
{
	try
	{
		// init constant buffers
		auto hr = this->m_cb_vs_vertexshader.Initialize(m_device.Get(), m_device_context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");
		hr = this->m_cb_vs_camlightmatrix.Initialize(this->m_device.Get(), this->m_device_context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");
		hr = this->m_cb_ps_light.Initialize(m_device.Get(), m_device_context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");
		hr = this->m_cb_ps_pointlight.Initialize(m_device.Get(), m_device_context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		// create katamari (player)
		if (!m_katamary.Initialize("Data/Objects/Samples/orange_disktexture.fbx", this->m_device.Get(), this->m_device_context.Get(), this->m_cb_vs_vertexshader))
			return false;
		m_katamary.CreateKatamari();

		// create light
		if (!m_sun.Initialize(0.5f, 100.0f))
			return false;
		this->m_cb_ps_light.data.diffuse_color = m_sun.diffuse_light_color;
		this->m_cb_ps_light.data.diffuse_strength = m_sun.diffuse_light_strength;
		this->m_cb_ps_light.data.light_direction = m_sun.GetDirection();
		// create landscape
		if (!m_land.Initialize(this->m_device.Get(), this->m_device_context.Get()))
			return false;

		if (!m_pointlight.Initialize(this->m_device.Get(), this->m_device_context.Get(), this->m_cb_vs_vertexshader))
			return false;

		for (int k = 0; k < g_numLights; ++k)
		{
			PointLightMesh pointlight;
			pointlight.Initialize(this->m_device.Get(), this->m_device_context.Get(), this->m_cb_vs_vertexshader);
			m_pointlights.push_back(pointlight);
		}

		// create collectable items
		for (int j = 0; j < g_numItems; ++j)
		{
			for (int i = 0; i < pathToItems.size(); ++i)
			{
				KatamariThing game_obj;
				game_obj.Initialize(pathToItems[i], this->m_device.Get(), this->m_device_context.Get(), this->m_cb_vs_vertexshader, true, pathToTextures[i]);
				game_obj.CreateUniqueWorld();
				m_items.push_back(game_obj);
			}
		};
		m_camera.SetPosition(0.0f, 0.0f, -2.0f);
		m_camera.SetProjectionValues(90.f, static_cast<float>(m_window_width) / static_cast<float>(m_window_height),
			0.1f, 10000.0f);
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void Graphics::UpdateConstantBuffers()
{
	//Update Constant Buffer(s)
	this->m_cb_ps_light.data.diffuse_color = m_sun.diffuse_light_color;
	this->m_cb_ps_light.data.diffuse_strength = m_sun.diffuse_light_strength;
	this->m_cb_ps_light.data.light_position = m_sun.GetPositionFloat3();
	this->m_cb_ps_light.data.ambient_color = m_sun.ambient_light_color;
	this->m_cb_ps_light.data.ambient_strength = m_sun.ambient_light_strength;
	this->m_cb_ps_light.data.camera_position = m_camera.GetPositionFloat3();
	this->m_cb_ps_light.data.specular_strength = m_sun.specular_strength;
	this->m_cb_ps_light.data.light_direction = m_sun.GetDirection();
	this->m_cb_ps_light.ApplyChanges();

	this->m_cb_vs_camlightmatrix.data.camLightViewMatrix = m_sun.GetViewMatrix();
	this->m_cb_vs_camlightmatrix.data.camLightProjMatrix = m_sun.GetProjectionMatrix();
	this->m_cb_vs_camlightmatrix.ApplyChanges();

	this->m_cb_ps_pointlight.data.cameraPosition = m_camera.GetPositionFloat3();
	this->m_cb_ps_pointlight.ApplyChanges();
}

void Graphics::RenderToTexture()
{
	m_shadow_map->SetShadowmapRenderTarget(this->m_device_context.Get());

	this->m_device_context->VSSetConstantBuffers(1, 1, this->m_cb_vs_camlightmatrix.GetAddressOf());
	// set shaders for shadow mapping
	this->m_device_context->VSSetShader(m_depth_vertexshader.GetShader(), NULL, 0);
	this->m_device_context->PSSetShader(m_depth_pixelshader.GetShader(), NULL, 0);
	{
		this->m_katamary.Draw(m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix());
		for (int i = 0; i < m_items.size(); ++i)
			m_items[i].Draw(m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix());
		this->m_land.Draw(this->m_cb_vs_vertexshader, m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix());
	}
}

void Graphics::RenderToGbuff()
{
	//unbind shader resource view with shadowmap from past pass so we will able to use it in next stages
	ID3D11ShaderResourceView* pNullSRV = NULL;
	m_device_context->PSSetShaderResources(1, 1, &pNullSRV);
	m_device_context->PSSetShaderResources(2, 1, &pNullSRV);
	m_device_context->PSSetShaderResources(3, 1, &pNullSRV);
	m_device_context->PSSetShaderResources(4, 1, &pNullSRV);
	//unbind render targets from past pass
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_device_context->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);

	//set default viewport
	this->m_device_context->RSSetViewports(1, &m_viewport);
	//set rendertargets
	m_device_context->OMSetRenderTargets(BUFFER_COUNT, m_gbuffer->m_renderTargetViewArray, this->m_depth_stencil_view.Get());

	float bgcolor[] = { 0.0f,0.0f,0.0f,1.0f };
	for (int i = 0; i < BUFFER_COUNT; ++i)
		m_device_context->ClearRenderTargetView(m_gbuffer->m_renderTargetViewArray[i], bgcolor);
	this->m_device_context->ClearDepthStencilView(this->m_depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->m_device_context->VSSetShader(m_deferred_vertexshader.GetShader(), nullptr, 0);
	this->m_device_context->PSSetShader(m_deferred_pixelshader.GetShader(), nullptr, 0);
	{
		this->m_katamary.Draw(m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix());
		/*m_bulb.Draw(m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix());*/
		this->m_land.Draw(this->m_cb_vs_vertexshader, m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix());
	}
	{
		for (int i = 0; i < m_items.size(); ++i)
		{
			m_items[i].Draw(m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix());
		}
	}
}

void Graphics::RenderToWindow()
{
	//disable z-buffer
	m_device_context->OMSetDepthStencilState(m_depth_disabled_stencil_state.Get(), 0);
	//unbind shader resource view with shadowmap from past pass so we will able to use it in next stages
	ID3D11ShaderResourceView* pNullSRV = NULL;
	m_device_context->PSSetShaderResources(1, 1, &pNullSRV);
	//unbind render targets from past pass
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_device_context->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);

	//set default rendertarget
	this->m_device_context->OMSetRenderTargets(1, this->m_render_target_view.GetAddressOf(), this->m_depth_stencil_view.Get());

	this->m_device_context->PSSetConstantBuffers(0, 1, this->m_cb_ps_light.GetAddressOf());
	this->m_device_context->PSSetConstantBuffers(1, 1, this->m_cb_vs_camlightmatrix.GetAddressOf());

	float bgcolor[] = { 0.0,0.0,0.0,1.0f };
	this->m_device_context->ClearRenderTargetView(this->m_render_target_view.Get(), bgcolor);
	this->m_device_context->ClearDepthStencilView(this->m_depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->m_device_context->PSSetShaderResources(1, 1, m_shadow_map->GetShaderResourceViewAddress());
	this->m_device_context->PSSetShaderResources(2, 1, &m_gbuffer->m_shaderResourceViewArray[0]);
	this->m_device_context->PSSetShaderResources(3, 1, &m_gbuffer->m_shaderResourceViewArray[1]);
	this->m_device_context->PSSetShaderResources(4, 1, &m_gbuffer->m_shaderResourceViewArray[2]);

	float blend[4] = { 1,1,1, 1 };
	this->m_device_context->OMSetBlendState(m_blend_state.Get(), blend, 0xFFF);
	this->m_device_context->RSSetState(m_rasterizer_state.Get());

	DoDirLightPass();
	//enable z-buff
	m_device_context->OMSetDepthStencilState(m_depth_stencil_state.Get(), 1);
	DoPointLightPass();
	this->m_device_context->OMSetBlendState(nullptr, blend, 0xFFF);
	//this->m_device_context->RSSetState(nullptr);
}

void Graphics::DoDirLightPass()
{
	ID3D11Buffer* nothing = 0;
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_device_context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	m_device_context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
	this->m_device_context->VSSetShader(m_vertexshader_dirlight_pass.GetShader(), nullptr, 0);
	this->m_device_context->PSSetShader(m_pixelshader_dirlight_pass.GetShader(), nullptr, 0);

	m_device_context->Draw(3, 0);
}

void Graphics::DoPointLightPass()
{
	for (int i = 0; i < m_pointlights.size(); ++i)
	{
		this->m_cb_ps_pointlight.data.lightColor = m_pointlights[i].GetLightColor();
		this->m_cb_ps_pointlight.data.lightPosition = m_pointlights[i].GetPositionFloat3();
		this->m_cb_ps_pointlight.data.lightRadius = m_pointlights[i].GetLightRadius();
		this->m_cb_ps_pointlight.ApplyChanges();

		this->m_device_context->PSSetConstantBuffers(0, 1, this->m_cb_ps_pointlight.GetAddressOf());
		this->m_device_context->VSSetShader(m_vertexshader_pointlight_pass.GetShader(), nullptr, 0);
		this->m_device_context->PSSetShader(m_pixelshader_pointlight_pass.GetShader(), nullptr, 0);

		m_pointlights[i].Draw(m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix());
	}
}

void Graphics::CheckCollision()
{
	for (int i = 0; i < m_items.size(); ++i)
	{
		if (!this->m_items[i].isChild)
		{
			if (m_katamary.FindCollision(&m_items[i]))
			{
				m_katamary.AddChild(&m_items[i]);
			}
		}
	}
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