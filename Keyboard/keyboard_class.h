//
// Created by mi on 06.08.2020.
//
#pragma once
#ifndef CG_KEYBOARD_CLASS_H
#define CG_KEYBOARD_CLASS_H
#include "keyboard_event.h"
#include <queue>

class KeyboardClass
{
public:
    KeyboardClass();
    bool KeyIsPressed(const unsigned char keycode);
    bool KeyBufferIsEmpty();  
    bool CharBufferIsEmpty();
    KeyboardEvent ReadKey();
    unsigned char ReadChar();
    void OnKeyPressed(const unsigned char key);
    void OnKeyReleased(const unsigned char key);
    void OnChar(const unsigned char key);
    void EnableAutoRepeatKeys();
    void DisableAutoRepeatKeys();
    void EnableAutoRepeatChars();
    void DisableAutoRepeatChars();
    bool IsKeysAutoRepeat();
    bool IsCharsAutoRepeat();
private:
    bool autoRepeatKeys = false;
    bool autoRepeatChars = false;
    bool keyStates[256];
    std::queue<KeyboardEvent> keyBuffer;
    std::queue<unsigned char> charBuffer;
};


#endif //CG_KEYBOARD_CLASS_H
