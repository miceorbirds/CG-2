#pragma once

#include "error_logger.h"

class WindowContainer;

class RenderWindow
{
public:
    bool Initialize(WindowContainer * pWindowContainer, HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
    bool ProcessMessages();
    HWND GetHWND() const;
    ~RenderWindow();
private:
    void RegisterWindowClass();
    HWND m_handle = nullptr; //Handle to this window
    HINSTANCE m_hInstance = nullptr; //Handle to application instance
    std::string m_window_title;
    std::wstring m_window_title_wide; //Wide string representation of window title
    std::string m_window_class;
    std::wstring m_window_class_wide; //Wide string representation of window class name
    int m_width = 0;
    int m_height = 0;
};
