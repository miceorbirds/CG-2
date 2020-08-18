#include "mouse_event.h"

MouseEvent::MouseEvent()
        :
        m_type(EventType::Invalid),
        x(0),
        y(0)
{}

MouseEvent::MouseEvent(EventType m_type, int x, int y)
        :
        m_type(m_type),
        x(x),
        y(y)
{}

bool MouseEvent::IsValid() const
{
    return this->m_type != EventType::Invalid;
}

MouseEvent::EventType MouseEvent::GetType() const
{
    return this->m_type;
}

MousePoint MouseEvent::GetPos() const
{
    return{ this->x,this->y };
}

int MouseEvent::GetPosX() const
{
    return this->x;
}

int MouseEvent::GetPosY() const
{
    return this->y;
}