#pragma once

#include "mouse_event.h"
#include <queue>

class MouseClass {
public:
    void OnLeftPressed(int x, int y);
    void OnLeftReleased(int x, int y);
    void OnRightPressed(int x, int y);
    void OnRightReleased(int x, int y);
    void OnMiddlePressed(int x, int y);
    void OnMiddleReleased(int x, int y);
    void OnWheelUp(int x, int y);
    void OnWheelDown(int x, int y);
    void OnMouseMove(int x, int y);
    void OnMouseMoveRaw(int x, int y);

    bool IsLeftDown();
    bool IsMiddleDown();
    bool IsRightDown();

    int GetPosX();
    int GetPosY();
    MousePoint GetPos();

    bool EventBufferIsEmpty();
    MouseEvent ReadEvent();

private:
    std::queue<MouseEvent> m_event_buffer;
    bool m_left_is_down = false;
    bool m_right_is_down = false;
    bool m_button_down = false;
    int x = 0;
    int y = 0;
};