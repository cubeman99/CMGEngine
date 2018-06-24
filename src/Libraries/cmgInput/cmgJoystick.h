#ifndef _CMG_INPUT_JOYSTICK_H_
#define _CMG_INPUT_JOYSTICK_H_

#include <cmgInput/cmgInputDevice.h>
#include <cmgInput/cmgDirectInputIncludes.h>
#include <cmgMath/types/cmgVector2f.h>


struct XboxControllerState
{
	Vector2f leftStick;
	Vector2f rightStick;
	float leftTrigger;
	float rightTrigger;

	union
	{
		struct
		{
			bool a;
			bool b;
			bool x;
			bool y;
			bool start;
			bool back;
			bool leftThumb;
			bool rightThumb;
			bool leftShoulder;
			bool rightShoulder;
			
			struct
			{
				bool up;
				bool down;
				bool left;
				bool right;
			} dpad;
		};

		struct
		{
			bool buttons[14];
		};
	};
};


class Joystick : public InputDevice
{
public:
	enum 
	{
		k_device_type = InputDeviceType::k_joystick
	};

	enum class Buttons
	{
		A,
		B,
		X,
		Y,
		Start,
		Back,
		LeftThumb,
		RightThumb,
		LeftShoulder,
		RightShoulder,
		DPadUp,
		DPadDown,
		DPadLeft,
		DPadRight,
	};

public:
	Joystick(InputManager* inputManager);
	~Joystick();

	void Reset();
	void Update();
	
	inline bool IsButtonDown(Buttons button) const
	{
		return m_xboxState.buttons[(uint32) button];
	}
	
	inline bool IsButtonPressed(Buttons button) const
	{
		return m_xboxState.buttons[(uint32) button] &&
			!m_xboxStatePrev.buttons[(uint32) button];
	}
	
	inline bool IsButtonReleased(Buttons button) const
	{
		return !m_xboxState.buttons[(uint32) button] &&
			m_xboxStatePrev.buttons[(uint32) button];
	}
	
	inline const XboxControllerState& GetStatePrev() const
	{
		return m_xboxState;
	}
	
	inline const XboxControllerState& GetState() const
	{
		return m_xboxState;
	}

private:
	void DoInitialize();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_joystick;
	HWND m_windowHandle;

public:
	XboxControllerState m_xboxStatePrev;
	XboxControllerState m_xboxState;
};


#endif // _CMG_INPUT_JOYSTICK_H_