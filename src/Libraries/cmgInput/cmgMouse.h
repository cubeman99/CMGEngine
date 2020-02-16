#ifndef _CMG_MOUSE_H_
#define _CMG_MOUSE_H_

#include <cmgInput/cmgInputDevice.h>
#include <cmgInput/cmgDirectInputIncludes.h>
#include <cmgMath/types/cmgVector3.h>

struct MouseButtons
{
	enum
	{
		none      = 0,
		left      = 1 << 0,
		right     = 1 << 1,
		middle    = 1 << 2,
		button4   = 1 << 3,
		button5   = 1 << 4,
		button6   = 1 << 5,
		button7   = 1 << 6,
		button8   = 1 << 7,
	};

	typedef int value_type;
};
	

struct MouseState
{
	MouseState() :
		buttons(0),
		location(0, 0, 0)
	{ }

	MouseButtons::value_type buttons;
	Vector3i location;
};


class Mouse : public InputDevice
{
public:
	static constexpr InputDeviceType k_device_type =
		InputDeviceType::k_mouse;

	typedef MouseButtons::value_type mouse_button;

public:
	Mouse(InputManager* inputManager);
	~Mouse();

	void Reset() override;
	void Update() override;
	
	const MouseState& GetMouseState()		const { return m_currentState; }
	const MouseState& GetPrevMouseState()	const { return m_previousState; }

	bool IsButtonDown(mouse_button button) const ;
	bool IsButtonPressed(mouse_button button) const;
	bool IsButtonReleased(mouse_button button) const;
	

private:
	void DoInitialize();
		

private:
	IDirectInput8*			m_directInput;
	IDirectInputDevice8*	m_mouse;
	HWND					m_windowHandle;
	DIMOUSESTATE2			m_mouseBuffer;
	MouseState				m_currentState;
	MouseState				m_previousState;

	static const size_t		s_mouseBufferSize = sizeof(DIMOUSESTATE2);
};


#endif // _CMG_MOUSE_H_