//
// Created by mi on 05.08.2020.
//

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
    RenderWindow render_window;
    KeyboardClass keyboard;
    MouseClass mouse;
    Graphics gfx;
private:

};
