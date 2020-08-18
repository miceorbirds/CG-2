#include "adapter_reader.h"

std::vector<AdapterData> AdapterReader::m_adapters;

std::vector<AdapterData> AdapterReader::GetAdapters()
{
    if (!m_adapters.empty()) //If already initialized
        return m_adapters;

    Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;

    // Create a DXGIFactory object.
    HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
    if (FAILED(hr))
    {
        ErrorLogger::Log(hr, "Failed to create DXGIFactory for enumerating adapters.");
        exit(-1);
    }

    IDXGIAdapter * pAdapter;
    UINT index = 0;
    while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter)))
    {
        m_adapters.emplace_back(pAdapter);
        index += 1;
    }
    return m_adapters;
}

AdapterData::AdapterData(IDXGIAdapter * pAdapter)
{
    this->pAdapter = pAdapter;
    HRESULT hr = pAdapter->GetDesc(&this->description);
    if (FAILED(hr))
    {
        ErrorLogger::Log(hr, "Failed to Get Description for IDXGIAdapter.");
    }
}

