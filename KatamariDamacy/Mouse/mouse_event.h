#pragma once

struct MousePoint
{
    int x;
    int y;
};

class MouseEvent
{
public:
    enum EventType
    {
        LPress,
        LRelease,
        RPress,
        RRelease,
        MPress,
        MRelease,
        WheelUp,
        WheelDown,
        Move,
        RAW_MOVE,
        Invalid
    };
private:
    EventType m_type;
    int x;
    int y;
public:
    MouseEvent();
    MouseEvent(const EventType m_type, const int x, const int y);
    bool IsValid() const;
    EventType GetType() const;
    MousePoint GetPos() const;
    int GetPosX() const;
    int GetPosY() const;
};

