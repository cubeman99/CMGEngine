#include "cmgKeyboard.h"
#include <cmgInput/cmgInputManager.h>
#include <stdio.h>


namespace
{
// Translate a DirectInput keycode to a our keycode type.
Keys TranslateKeyCode(DWORD dwOfs)
{
	switch (dwOfs)
	{
	case DIK_ESCAPE:      return Keys::escape;
	case DIK_1:           return Keys::n1;
	case DIK_2:           return Keys::n2;
	case DIK_3:           return Keys::n3;
	case DIK_4:           return Keys::n4;
	case DIK_5:           return Keys::n5;
	case DIK_6:           return Keys::n6;
	case DIK_7:           return Keys::n7;
	case DIK_8:           return Keys::n8;
	case DIK_9:           return Keys::n9;
	case DIK_0:           return Keys::n0;
	case DIK_MINUS:       return Keys::minus;
	case DIK_EQUALS:      return Keys::equals;
	case DIK_BACK:        return Keys::backspace;
	case DIK_TAB:         return Keys::tab;
	case DIK_Q:           return Keys::q;
	case DIK_W:           return Keys::w;
	case DIK_E:           return Keys::e;
	case DIK_R:           return Keys::r;
	case DIK_T:           return Keys::t;
	case DIK_Y:           return Keys::y;
	case DIK_U:           return Keys::u;
	case DIK_I:           return Keys::i;
	case DIK_O:           return Keys::o;
	case DIK_P:           return Keys::p;
	case DIK_LBRACKET:    return Keys::left_bracket;
	case DIK_RBRACKET:    return Keys::right_bracket;
	case DIK_RETURN:      return Keys::enter;
	case DIK_LCONTROL:    return Keys::left_control;
	case DIK_A:           return Keys::a;
	case DIK_S:           return Keys::s;
	case DIK_D:           return Keys::d;
	case DIK_F:           return Keys::f;
	case DIK_G:           return Keys::g;
	case DIK_H:           return Keys::h;
	case DIK_J:           return Keys::j;
	case DIK_K:           return Keys::k;
	case DIK_L:           return Keys::l;
	case DIK_SEMICOLON:   return Keys::semicolon;
	case DIK_APOSTROPHE:  return Keys::quote;
	case DIK_GRAVE:       return Keys::grave;
	case DIK_LSHIFT:      return Keys::left_shift;
	case DIK_BACKSLASH:   return Keys::backslash;
	case DIK_Z:           return Keys::z;
	case DIK_X:           return Keys::x;
	case DIK_C:           return Keys::c;
	case DIK_V:           return Keys::v;
	case DIK_B:           return Keys::b;
	case DIK_N:           return Keys::n;
	case DIK_M:           return Keys::m;
	case DIK_COMMA:       return Keys::comma;
	case DIK_PERIOD:      return Keys::period;
	case DIK_SLASH:       return Keys::slash;
	case DIK_RSHIFT:      return Keys::right_shift;
	case DIK_MULTIPLY:    return Keys::multiply_keypad;
	case DIK_LMENU:       return Keys::left_alt;
	case DIK_SPACE:       return Keys::space;
	case DIK_CAPITAL:     return Keys::capital;
	case DIK_F1:          return Keys::f1;
	case DIK_F2:          return Keys::f2;
	case DIK_F3:          return Keys::f3;
	case DIK_F4:          return Keys::f4;
	case DIK_F5:          return Keys::f5;
	case DIK_F6:          return Keys::f6;
	case DIK_F7:          return Keys::f7;
	case DIK_F8:          return Keys::f8;
	case DIK_F9:          return Keys::f9;
	case DIK_F10:         return Keys::f10;
	case DIK_NUMLOCK:     return Keys::numlock;
	case DIK_SCROLL:      return Keys::scroll_lock;
	case DIK_NUMPAD7:     return Keys::num7;
	case DIK_NUMPAD8:     return Keys::num8;
	case DIK_NUMPAD9:     return Keys::num9;
	case DIK_SUBTRACT:    return Keys::minus_keypad;
	case DIK_NUMPAD4:     return Keys::num4;
	case DIK_NUMPAD5:     return Keys::num5;
	case DIK_NUMPAD6:     return Keys::num6;
	case DIK_ADD:         return Keys::add_keypad;
	case DIK_NUMPAD1:     return Keys::num1;
	case DIK_NUMPAD2:     return Keys::num2;
	case DIK_NUMPAD3:     return Keys::num3;
	case DIK_NUMPAD0:     return Keys::num0;
	case DIK_DECIMAL:     return Keys::period_keypad;
	case DIK_F11:         return Keys::f11;
	case DIK_F12:         return Keys::f12;
	case DIK_F13:         return Keys::f13;
	case DIK_F14:         return Keys::f14;
	case DIK_F15:         return Keys::f15;
	case DIK_PREVTRACK:   return Keys::prev_track;
	case DIK_NEXTTRACK:   return Keys::next_track;
	case DIK_NUMPADENTER: return Keys::enter_keypad;
	case DIK_RCONTROL:    return Keys::right_control;
	case DIK_MUTE:        return Keys::mute;
	case DIK_CALCULATOR:  return Keys::calculator;
	case DIK_PLAYPAUSE:   return Keys::play_pause;
	case DIK_MEDIASTOP:   return Keys::media_stop;
	case DIK_VOLUMEDOWN:  return Keys::volume_down;
	case DIK_VOLUMEUP:    return Keys::volume_up;
	case DIK_WEBHOME:     return Keys::web_home;
	case DIK_DIVIDE:      return Keys::divide_keypad;
	case DIK_SYSRQ:       return Keys::sysrq;
	case DIK_RMENU:       return Keys::right_alt;
	case DIK_PAUSE:       return Keys::pause;
	case DIK_HOME:        return Keys::home;
	case DIK_UP:          return Keys::up;
	case DIK_PRIOR:       return Keys::page_up;
	case DIK_LEFT:        return Keys::left;
	case DIK_RIGHT:       return Keys::right;
	case DIK_END:         return Keys::end;
	case DIK_DOWN:        return Keys::down;
	case DIK_NEXT:        return Keys::page_down;
	case DIK_INSERT:      return Keys::insert;
	case DIK_DELETE:      return Keys::k_delete;
	case DIK_LWIN:        return Keys::left_sys;
	case DIK_RWIN:        return Keys::right_sys;
	case DIK_POWER:       return Keys::power_sys;
	case DIK_SLEEP:       return Keys::sleep_sys;
	case DIK_WAKE:        return Keys::wake_sys;
	default:              return Keys::none;
	}
}
};

Keyboard::Keyboard(InputManager* inputManager) :
	m_windowHandle(inputManager->GetWindowHandle()),
	m_directInput(inputManager->GetDirectInput()),
	m_keyboard(NULL)
{
	//core::fill(m_buffer, m_buffer + core_utils::ArraySize(m_buffer), false);
	memset(m_buffer, 0, sizeof(m_buffer));
	memset(m_bufferPrev, 0, sizeof(m_buffer));
	DoInitialize();
}

Keyboard::~Keyboard()
{
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = CMG_NULL;
	}
}


//-----------------------------------------------------------------------------
// Input state checks
//-----------------------------------------------------------------------------

bool Keyboard::IsKeyDown(Keys key) const
{
	return m_buffer[(int32) key];
}

bool Keyboard::IsKeyPressed(Keys key) const
{
	return (m_buffer[(int32) key] && !m_bufferPrev[(int32) key]);
}

bool Keyboard::IsKeyReleased(Keys key) const
{
	return (!m_buffer[(int32) key] && m_bufferPrev[(int32) key]);
}


void Keyboard::AddEventHandler(IKeyboardEventHandler* handler)
{
	m_eventHandlers.push_back(handler);
}


void Keyboard::InjectKeyDownEvent(Keys key)
{
	m_buffer[(int32) key] = true;
}

void Keyboard::InjectKeyUpEvent(Keys key)
{
	m_buffer[(int32) key] = false;
}

//-----------------------------------------------------------------------------
// Input Update
//-----------------------------------------------------------------------------

void Keyboard::Reset()
{
	memcpy(m_bufferPrev, m_buffer, sizeof(m_buffer));
	memset(m_buffer, 0, sizeof(m_buffer));
}

void Keyboard::Update()
{
	memcpy(m_bufferPrev, m_buffer, sizeof(m_buffer));
	return;
	// Check if the keyboard has lost input or isn't acquired
	HRESULT hRes = m_keyboard->GetDeviceState(s_bufferSize, &m_rawBuffer);
	if (hRes == DIERR_INPUTLOST || hRes == DIERR_NOTACQUIRED)
	{
		// Try to re-acquire the device. If this fails, then another
		// application has priority over the keyboard! This means this
		// application's window is not in the foreground.
		hRes = m_keyboard->Acquire();
		if (hRes == DI_OK)
			m_keyboard->GetDeviceState(sizeof(s_bufferSize), &m_rawBuffer);
	}
	if (hRes != DI_OK)
		memset(m_rawBuffer, 0, sizeof(m_rawBuffer));

	// Translate the keycodes to put into our buffer
	for (DWORD i = 0; i < s_bufferSize; ++i)
	{
		Keys key = TranslateKeyCode(i);
		if ((m_rawBuffer[i] & 0x80) != 0)
		{
			m_buffer[(int32) key] = true;

			if (!m_bufferPrev[(int32) key])
			{
				for (auto handler : m_eventHandlers)
					handler->OnKeyDown(key);
			}
		}
		else
		{
			m_buffer[(int32) key] = false;

			if (m_bufferPrev[(int32) key])
			{
				for (auto handler : m_eventHandlers)
					handler->OnKeyUp(key);
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Private Methods
//-----------------------------------------------------------------------------

void Keyboard::DoInitialize()
{
	// Initialize the direct input interface for the keyboard.
	HRESULT result = m_directInput->CreateDevice(
		GUID_SysKeyboard, &m_keyboard, CMG_NULL);
	if (FAILED(result))
	{
		CMG_LOG_ERROR() << "Failure to initialize keyboard";
		return;
	}

	// Set the data format. In this case since it is a keyboard we can use the
	// predefined data format.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		CMG_LOG_ERROR() << "Could not set keyboard device format";
		return;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = m_keyboard->SetCooperativeLevel(
		m_windowHandle, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		CMG_LOG_ERROR() << "Keyboard cooperative level settings error";
		return;
	}
	/*
	// Now acquire the keyboard.
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		CMG_LOG_ERROR() << "Failure to acquire keyboard";
		return;
	}*/
}

