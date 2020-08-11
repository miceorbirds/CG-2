//
// Created by mi on 06.08.2020.
//
#pragma once
#ifndef CG_KEYBOARD_EVENT_H
#define CG_KEYBOARD_EVENT_H

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
    KeyboardEvent(const EventType, const unsigned char key);
    bool IsPress() const;
    bool IsRelease() const;
    bool IsValid() const;
    unsigned char GetKeyCode() const;

private:
    EventType type;
    unsigned char key;
} ;

#endif //CG_KEYBOARD_EVENT_H
