#pragma once

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
	void OnKeyPressed(const unsigned char m_key);
	void OnKeyReleased(const unsigned char m_key);
	void OnChar(const unsigned char m_key);
	void EnableAutoRepeatKeys();
	void DisableAutoRepeatKeys();
	void EnableAutoRepeatChars();
	void DisableAutoRepeatChars();
	bool IsKeysAutoRepeat();
	bool IsCharsAutoRepeat();
private:
	bool m_auto_repeat_keys = false;
	bool m_auto_repeat_chars = false;
	bool m_key_states[256];
	std::queue<KeyboardEvent> m_keyBuffer;
	std::queue<unsigned char> m_charBuffer;
};
