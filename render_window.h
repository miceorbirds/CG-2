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
    HWND handle_ = nullptr; //Handle to this window
    HINSTANCE hInstance_ = nullptr; //Handle to application instance
    std::string window_title_;
    std::wstring window_title_wide_; //Wide string representation of window title
    std::string window_class_;
    std::wstring window_class_wide_; //Wide string representation of window class name
    int width_ = 0;
    int height_ = 0;
};
