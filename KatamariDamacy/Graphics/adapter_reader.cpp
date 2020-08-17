#include "adapter_reader.h"

std::vector<AdapterData> AdapterReader::adapters_;

std::vector<AdapterData> AdapterReader::GetAdapters()
{
    if (!adapters_.empty()) //If already initialized
        return adapters_;

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
        adapters_.emplace_back(pAdapter);
        index += 1;
    }
    return adapters_;
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

