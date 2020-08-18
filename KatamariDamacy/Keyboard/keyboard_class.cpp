#include "keyboard_class.h"

KeyboardClass::KeyboardClass()
{
    for (int i = 0; i < 256; i++)
        this->m_key_states[i] = false; //Initialize all key states to off (false)
}

bool KeyboardClass::KeyIsPressed(const unsigned char keycode)
{
    return this->m_key_states[keycode];
}

bool KeyboardClass::KeyBufferIsEmpty()
{
    return this->m_keyBuffer.empty();
}

bool KeyboardClass::CharBufferIsEmpty()
{
    return this->m_charBuffer.empty();
}

KeyboardEvent KeyboardClass::ReadKey()
{
    if (this->m_keyBuffer.empty()) //If no keys to be read?
    {
        return KeyboardEvent(); //return empty keyboard event
    }
    else
    {
        KeyboardEvent e = this->m_keyBuffer.front(); //Get first Keyboard Event from queue
        this->m_keyBuffer.pop(); //Remove first item from queue
        return e; //Returns keyboard event
    }
}

unsigned char KeyboardClass::ReadChar()
{
    if (this->m_charBuffer.empty()) //If no keys to be read?
    {
        return 0u; //return 0 (NULL char)
    }
    else
    {
        unsigned char e = this->m_charBuffer.front(); //Get first char from queue
        this->m_charBuffer.pop(); //Remove first char from queue
        return e; //Returns char
    }
}

void KeyboardClass::OnKeyPressed(const unsigned char m_key)
{
    this->m_key_states[m_key] = true;
    this->m_keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, m_key));
}

void KeyboardClass::OnKeyReleased(const unsigned char m_key)
{
    this->m_key_states[m_key] = false;
    this->m_keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, m_key));
}

void KeyboardClass::OnChar(const unsigned char m_key)
{
    this->m_charBuffer.push(m_key);
}

void KeyboardClass::EnableAutoRepeatKeys()
{
    this->m_auto_repeat_keys = true;
}

void KeyboardClass::DisableAutoRepeatKeys()
{
    this->m_auto_repeat_keys = false;
}

void KeyboardClass::EnableAutoRepeatChars()
{
    this->m_auto_repeat_chars = true;
}

void KeyboardClass::DisableAutoRepeatChars()
{
    this->m_auto_repeat_chars = false;
}

bool KeyboardClass::IsKeysAutoRepeat()
{
    return this->m_auto_repeat_keys;
}

bool KeyboardClass::IsCharsAutoRepeat()
{
    return this->m_auto_repeat_chars;
}