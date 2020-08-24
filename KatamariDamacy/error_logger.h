#pragma once

#include <Windows.h>
#include "string_converter.h"

class ErrorLogger
{
public:
    static void Log(const std::string& message);
    static void Log(HRESULT hr, std::string message);
    static void Log(HRESULT hr, std::wstring message);

};