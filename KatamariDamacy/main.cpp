#include "engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					  _In_opt_ HINSTANCE hPrevInstance,
					  _In_ PWSTR    lpCmdLine,
					  _In_ int       nCmdShow)
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to call CoInitialize.");
		return -1;
	}
	
	Engine engine;
	if (engine.Initialize(hInstance, "Hello World", "MyWindowClass", 800, 600))
	{
		while(engine.ProcessMessages())
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