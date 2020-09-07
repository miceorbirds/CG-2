#pragma once

#include "com_exception.h"
#include <Windows.h>

class ErrorLogger
{
public:
    static void Log(const std::string& message);
    static void Log(HRESULT hr, std::string message);
    static void Log(HRESULT hr, std::wstring message);
    static void Log(COMException& exception);
};