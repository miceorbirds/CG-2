#include "keyboard_event.h"

KeyboardEvent::KeyboardEvent()
        :
        m_type(EventType::Invalid),
        m_key(0u)
{
}

KeyboardEvent::KeyboardEvent(const EventType m_type, const unsigned char m_key)
        :
        m_type(m_type),
        m_key(m_key)
{
}

bool KeyboardEvent::IsPress() const
{
    return this->m_type == EventType::Press;
}

bool KeyboardEvent::IsRelease() const
{
    return this->m_type == EventType::Release;
}

bool KeyboardEvent::IsValid() const
{
    return this->m_type != EventType::Invalid;
}

unsigned char KeyboardEvent::GetKeyCode() const
{
    return this->m_key;
}