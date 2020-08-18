#include "mouse_class.h"

void MouseClass::OnLeftPressed(int x, int y)
{
    this->m_left_is_down = true;
    MouseEvent me(MouseEvent::EventType::LPress, x, y);
    this->m_event_buffer.push(me);
}

void MouseClass::OnLeftReleased(int x, int y)
{
    this->m_left_is_down = false;
    this->m_event_buffer.push(MouseEvent(MouseEvent::EventType::LRelease, x, y));
}

void MouseClass::OnRightPressed(int x, int y)
{
    this->m_right_is_down = true;
    this->m_event_buffer.push(MouseEvent(MouseEvent::EventType::RPress, x, y));
}

void MouseClass::OnRightReleased(int x, int y)
{
    this->m_right_is_down = false;
    this->m_event_buffer.push(MouseEvent(MouseEvent::EventType::RRelease, x, y));
}

void MouseClass::OnMiddlePressed(int x, int y)
{
    this->m_button_down = true;
    this->m_event_buffer.push(MouseEvent(MouseEvent::EventType::MPress, x, y));
}

void MouseClass::OnMiddleReleased(int x, int y)
{
    this->m_button_down = false;
    this->m_event_buffer.push(MouseEvent(MouseEvent::EventType::MRelease, x, y));
}

void MouseClass::OnWheelUp(int x, int y)
{
    this->m_event_buffer.push(MouseEvent(MouseEvent::EventType::WheelUp, x, y));
}

void MouseClass::OnWheelDown(int x, int y)
{
    this->m_event_buffer.push(MouseEvent(MouseEvent::EventType::WheelDown, x, y));
}

void MouseClass::OnMouseMove(int x, int y)
{
    this->x = x;
    this->y = y;
    this->m_event_buffer.push(MouseEvent(MouseEvent::EventType::Move, x, y));
}

bool MouseClass::IsLeftDown()
{
    return this->m_left_is_down;
}

bool MouseClass::IsMiddleDown()
{
    return this->m_button_down;
}

bool MouseClass::IsRightDown()
{
    return this->m_right_is_down;
}

int MouseClass::GetPosX()
{
    return this->x;
}

int MouseClass::GetPosY()
{
    return this->y;
}

MousePoint MouseClass::GetPos()
{
    return{ this->x, this->y };
}

bool MouseClass::EventBufferIsEmpty()
{
    return this->m_event_buffer.empty();
}

MouseEvent MouseClass::ReadEvent()
{
    if (this->m_event_buffer.empty())
    {
        return MouseEvent();
    } else
    {
        MouseEvent e = this->m_event_buffer.front(); //Get first event from buffer
        this->m_event_buffer.pop(); //Remove first event from buffer
        return e;
    }
}

void MouseClass::OnMouseMoveRaw(int x, int y)
{
    this->m_event_buffer.push(MouseEvent(MouseEvent::EventType::RAW_MOVE, x, y));
}
