//
// Created by mi on 04.08.2020.
//
#pragma once

// with this macro error.ErrorMessage() works fine.
// For more info look at: http://www.cplusplus.com/forum/articles/16820/
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#include <Windows.h>

#include "string_converter.h"

class ErrorLogger
{
public:
    static void Log(const std::string& message);
    static void Log(HRESULT hr, std::string message);
};