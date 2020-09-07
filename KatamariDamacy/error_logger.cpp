#include "error_logger.h"
#include <comdef.h>

void ErrorLogger::Log(const std::string& message)
{
	auto error_message = "Error: " + message;
    MessageBoxA(nullptr, error_message.c_str(), "Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::string message)
{
    _com_error error(hr);
    auto error_message = L"Error: " + StringConverter::StringToWide(message) + L"\n" + error.ErrorMessage();
    MessageBoxW(nullptr, error_message.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::wstring message)
{
    _com_error error(hr);
    auto error_message = L"Error: " + message + L"\n" + error.ErrorMessage();
    MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::Log(COMException& exception)
{
    std::wstring error_message = exception.what();
    MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}