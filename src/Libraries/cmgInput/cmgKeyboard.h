#ifndef _CMG_KEYBOARD_H_
#define _CMG_KEYBOARD_H_

#include <cmgInput/cmgInputDevice.h>
#include <cmgInput/cmgDirectInputIncludes.h>
#include <cmgCore/containers/cmgArray.h>

enum class Keys
{
	none = 0, // unsupported key was pressed

	space, enter, escape, tab, backspace,
	right_shift, left_shift, right_control, left_control, right_alt, left_alt,
	capital, numlock, scroll_lock, sysrq, pause,
	left, right, up, down,
	insert, k_delete, home, end, page_up, page_down,
	n1, n2, n3, n4, n5, n6, n7, n8, n9, n0, // digits
	a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
	f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24,
	minus, equals, left_bracket, right_bracket, semicolon, quote, grave, backslash, comma, period, slash,
	num0, num1, num2, num3, num4, num5, num6, num7, num8, num9,
	multiply_keypad, minus_keypad, add_keypad, period_keypad, divide_keypad, slash_keypad, enter_keypad,
	prev_track, next_track, play_pause, media_stop, volume_down, volume_up, mute, web_home, calculator, 
	left_sys, right_sys, power_sys, sleep_sys, wake_sys,
	count,
};

class IKeyboardEventHandler
{
public:
	virtual void OnKeyDown(Keys key) {}
	virtual void OnKeyUp(Keys key) {}
};

class Keyboard : public InputDevice
{
public:
	static constexpr InputDeviceType k_device_type =
		InputDeviceType::k_keyboard;

public:
	Keyboard(InputManager* inputManager);
	~Keyboard();

	void Reset();
	void Update();
	bool IsKeyDown(Keys key) const;
	bool IsKeyPressed(Keys key) const;
	bool IsKeyReleased(Keys key) const;

	void AddEventHandler(IKeyboardEventHandler* handler);
		
	void InjectKeyDownEvent(Keys key);
	void InjectKeyUpEvent(Keys key);

private:
	void DoInitialize();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	HWND m_windowHandle;

	bool m_buffer[(int32) Keys::count];
	bool m_bufferPrev[(int32) Keys::count];
	static const DWORD s_bufferSize = 256;
	uint8 m_rawBuffer[s_bufferSize];
	Array<IKeyboardEventHandler*> m_eventHandlers;
};


#endif // _CMG_KEYBOARD_H_