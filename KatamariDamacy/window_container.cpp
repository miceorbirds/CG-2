#include "window_container.h"

WindowContainer::WindowContainer()
{
    static bool raw_input_initialized = false;
    if (!raw_input_initialized)
    {
        RAWINPUTDEVICE rid;

        rid.usUsagePage = 0x01; //Mouse
        rid.usUsage = 0x02;
        rid.dwFlags = 0;
        rid.hwndTarget = nullptr;

        if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
        {
            ErrorLogger::Log(GetLastError(), "Failed to register raw input devices.");
            exit(-1);
        }

        raw_input_initialized = true;
    }
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WindowContainer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
        return true;
    switch (uMsg)
    {
        case WM_KEYDOWN:
        {
            auto keycode = static_cast<unsigned char>(wParam);
            if (m_keyboard.IsKeysAutoRepeat())
            {
                m_keyboard.OnKeyPressed(keycode);
            }
            else
            {
                const bool wasPressed = lParam & 0x40000000;
                if (!wasPressed)
                {
                    m_keyboard.OnKeyPressed(keycode);
                }
            }
            return 0;
        }
        case WM_KEYUP:
        {
            auto keycode = static_cast<unsigned char>(wParam);
            m_keyboard.OnKeyReleased(keycode);
            return 0;
        }
        case WM_CHAR:
        {
            auto ch = static_cast<unsigned char>(wParam);
            if (m_keyboard.IsCharsAutoRepeat())
            {
                m_keyboard.OnChar(ch);
            }
            else
            {
                const bool wasPressed = lParam & 0x40000000;
                if (!wasPressed)
                {
                    m_keyboard.OnChar(ch);
                }
            }
            return 0;
        }
        //Mouse Messages
        case WM_MOUSEMOVE:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            m_mouse.OnMouseMove(x, y);
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            m_mouse.OnLeftPressed(x, y);
            return 0;
        }
        case WM_RBUTTONDOWN:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            m_mouse.OnRightPressed(x, y);
            return 0;
        }
        case WM_MBUTTONDOWN:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            m_mouse.OnMiddlePressed(x, y);
            return 0;
        }
        case WM_LBUTTONUP:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            m_mouse.OnLeftReleased(x, y);
            return 0;
        }
        case WM_RBUTTONUP:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            m_mouse.OnRightReleased(x, y);
            return 0;
        }
        case WM_MBUTTONUP:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            m_mouse.OnMiddleReleased(x, y);
            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            {
                m_mouse.OnWheelUp(x, y);
            }
            else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
            {
                m_mouse.OnWheelDown(x, y);
            }
            return 0;
        }
        case WM_INPUT:
        {
            UINT dataSize;
            GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &dataSize, sizeof(RAWINPUTHEADER)); //Need to populate data size first

            if (dataSize > 0)
            {
                std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
                if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
                {
                    auto* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
                    if (raw->header.dwType == RIM_TYPEMOUSE)
                    {
                        m_mouse.OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
                    }
                }
            }
            return DefWindowProc(hwnd, uMsg, wParam, lParam); //Need to call DefWindowProc for WM_INPUT messages
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

