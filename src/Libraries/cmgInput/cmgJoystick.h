#ifndef _CMG_INPUT_JOYSTICK_H_
#define _CMG_INPUT_JOYSTICK_H_

#include <cmgCore/containers/cmgArray.h>
#include <cmgCore/string/cmgString.h>
#include <cmgInput/cmgInputDevice.h>
#include <cmgInput/cmgDirectInputIncludes.h>
#include <cmgMath/types/cmgVector2f.h>
#include <cmgMath/types/cmgVector3f.h>

struct InputDeviceInfo;

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
	static constexpr InputDeviceType k_device_type =
		InputDeviceType::k_joystick;

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

	struct AxisState
	{
		float position = 0.0f;
	};

	struct POVState
	{
		bool centered = true;
		float angle = 0.0f;
	};

	struct ButtonState
	{
		bool down = true;
	};

	struct Axis
	{
		AxisState state;
		uint32 index = 0;
		uint32 offset = 0;
		String name;
	};

	struct POV
	{
		POVState state;
		uint32 index = 0;
		String name;
	};

	struct Button
	{
		ButtonState state;
		uint32 index = 0;
		String name;
	};

public:
	Joystick(InputManager* inputManager, const InputDeviceInfo& deviceInfo);
	~Joystick();

	bool IsButtonDown(Buttons button) const;
	bool IsButtonPressed(Buttons button) const;
	bool IsButtonReleased(Buttons button) const;
	const XboxControllerState& GetStatePrev() const;
	const XboxControllerState& GetState() const;
	uint32 GetNumAxes() const;
	uint32 GetNumButtons() const;
	uint32 GetNumPOVs() const;
	const AxisState& GetAxisState(uint32 axisIndex) const;
	const ButtonState& GetButtonState(uint32 buttonIndex) const;
	const POVState& GetPOVState(uint32 povIndex) const;
	float GetAxisPosition(uint32 axisIndex) const;

	void Reset();
	void Update();

private:
	void DoInitialize(const InputDeviceInfo& deviceInfo);
	static BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* instance, void* joystick);

	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_joystick;
	HWND m_windowHandle;
	Array<Axis> m_axes;
	Array<Button> m_buttons;
	Array<POV> m_povs;
	XboxControllerState m_xboxStatePrev;
	XboxControllerState m_xboxState;
	DIJOYSTATE2 m_dxState;
};


#endif // _CMG_INPUT_JOYSTICK_H_