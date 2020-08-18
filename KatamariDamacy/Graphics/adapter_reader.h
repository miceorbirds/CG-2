#pragma once

#include "../error_logger.h"
#include <d3d11.h>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"DXGI.lib")
#include <wrl/client.h>
#include <vector> 

class AdapterData
{
public:
    AdapterData(IDXGIAdapter * pAdapter);
    IDXGIAdapter * pAdapter = nullptr;
    DXGI_ADAPTER_DESC description{ 0 };
};

class AdapterReader
{
public:
    static std::vector<AdapterData> GetAdapters();
private:
    static std::vector<AdapterData> m_adapters;
};



