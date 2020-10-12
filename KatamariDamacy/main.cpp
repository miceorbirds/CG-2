#include "engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	try
	{
		auto hr = CoInitialize(NULL);
		COM_ERROR_IF_FAILED(hr, "Failed to call CoInitialize.");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return -1;
	}

	Engine engine;

	if (engine.Initialize(hInstance, "Hello Katamari", "MyWindowClass", 1080, 720))
	{
		while (engine.ProcessMessages())
		{
			engine.Update();
			engine.RenderFrame();
		}
	}
	return 0;
}

// INPUT ASSEMBLER - COMPLETED (CreateInputLayout)
// VERTEX SHADER - COMPLETED (CreateVertexShader)
// RASTERIZER - COMPLETED (RSSetViewports)
// PIXEL SHADER - COMPLETED (CreatePixelShader)
// OUTPUT MERGER - COMPLETED (OMSetRenderTargets)