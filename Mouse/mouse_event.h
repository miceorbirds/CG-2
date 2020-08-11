//
// Created by mi on 07.08.2020.
//
#pragma once
#ifndef CG_MOUSE_EVENT_H
#define CG_MOUSE_EVENT_H


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
    EventType type;
    int x;
    int y;
public:
    MouseEvent();
    MouseEvent(const EventType type, const int x, const int y);
    bool IsValid() const;
    EventType GetType() const;
    MousePoint GetPos() const;
    int GetPosX() const;
    int GetPosY() const;
};


#endif //CG_MOUSE_EVENT_H
