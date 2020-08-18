#pragma once

class KeyboardEvent
{
public:
    enum EventType
    {
        Press,
        Release,
        Invalid
    };

    KeyboardEvent();
    KeyboardEvent(const EventType, const unsigned char m_key);
    bool IsPress() const;
    bool IsRelease() const;
    bool IsValid() const;
    unsigned char GetKeyCode() const;

private:
    EventType m_type;
    unsigned char m_key;
};

