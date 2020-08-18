#pragma once

#include <memory>
#include "render_window.h"
#include "Keyboard/keyboard_class.h"
#include "Mouse/mouse_class.h"
#include "Graphics/graphics.h"


class WindowContainer
{
public:
    WindowContainer();
    LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
    RenderWindow m_render_window;
    KeyboardClass m_keyboard;
    MouseClass m_mouse;
    Graphics m_gfx;
private:

};
