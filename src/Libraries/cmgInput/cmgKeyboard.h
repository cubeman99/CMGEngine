#ifndef _CMG_KEYBOARD_H_
#define _CMG_KEYBOARD_H_

#include <cmgInput/cmgInputDevice.h>
#include <cmgInput/cmgDirectInputIncludes.h>
#include <cmgCore/containers/cmgArray.h>

struct Keys
{
	enum
	{
		none = 0, // unsupported key was pressed

		escape,
		n1, n2, n3, n4, n5, n6, n7, n8, n9, n0, // digits
		minus, equals, backspace, tab,
		q, w, e, r, t, y, u, i, o, p,
		left_bracket, right_bracket,
		enter,
		left_control,
		a, s, d, f, g, h, j, k, l,
		semicolon, apostrophe, grave, left_shift, backslash,
		z, x, c, v, b, n, m,
		comma, period, slash, right_shift,
		multiply_keypad, left_alt, space, capital,
		f1, f2, f3, f4, f5, f6, f7, f8, f9, f10,
		numlock, scroll_lock,
		num7, num8, num9, minus_keypad,
		num4, num5, num6, add_keypad,
		num1, num2, num3, num0, period_keypad,
		f11, f12, f13, f14, f15,
		prev_track, next_track,
		enter_keypad,
		right_control, mute, calculator, play_pause, media_stop,
		volume_down, volume_up, web_home,
		divide_keypad, slash_keypad,
		sysrq, right_alt, pause, home,
		up, page_up, left, right, end, down, page_down, insert, k_delete,
		left_sys, right_sys, power_sys, sleep_sys, wake_sys,

		count,
	};

	typedef int value_type;
};

class IKeyboardEventHandler
{
public:
	virtual void OnKeyDown(uint32 key) {}
	virtual void OnKeyUp(uint32 key) {}
};

class Keyboard : public InputDevice
{
public:
	enum 
	{
		k_device_type = InputDeviceType::k_keyboard
	};

	typedef Keys::value_type key_type;

public:
	Keyboard(InputManager* inputManager);
	~Keyboard();

	void Reset();
	void Update();
	bool IsKeyDown(key_type key) const;
	bool IsKeyPressed(key_type key) const;
	bool IsKeyReleased(key_type key) const;

	void AddEventHandler(IKeyboardEventHandler* handler);
		
private:
	void DoInitialize();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	HWND m_windowHandle;

	bool m_buffer[Keys::count];
	bool m_bufferPrev[Keys::count];
	static const DWORD s_bufferSize = 256;
	uint8 m_rawBuffer[s_bufferSize];
	Array<IKeyboardEventHandler*> m_eventHandlers;
};


#endif // _CMG_KEYBOARD_H_