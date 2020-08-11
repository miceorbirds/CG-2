//
// Created by mi on 07.08.2020.
//
#pragma once
#ifndef CG_ADAPTER_READER_H
#define CG_ADAPTER_READER_H


#include "../error_logger.h"
#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <vector>
//#include "vertex.h"

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
    static std::vector<AdapterData> adapters_;
};


#endif //CG_ADAPTER_READER_H
