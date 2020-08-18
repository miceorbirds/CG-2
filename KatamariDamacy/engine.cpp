#include "engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
    if (!this->m_render_window.Initialize(this, hInstance, window_title, window_class, width, height))
    {
        return false;
    };
    if (!m_gfx.Initialize(this->m_render_window.GetHWND(), width, height))
    {
        return false;
    }
    return true;
}

bool Engine::ProcessMessages()
{
    return this->m_render_window.ProcessMessages();
}

void Engine::Update() {
    while (!m_keyboard.CharBufferIsEmpty())
    {
        unsigned char ch = m_keyboard.ReadChar();
    }

    while (!m_keyboard.KeyBufferIsEmpty())
    {
        KeyboardEvent kbe = m_keyboard.ReadKey();
        unsigned char keycode = kbe.GetKeyCode();
    }
    while (!m_mouse.EventBufferIsEmpty())
    {
       MouseEvent me = m_mouse.ReadEvent();
    }
}

void Engine::Renderframe()
{
    m_gfx.RenderFrame();
}
