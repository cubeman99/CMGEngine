#include "cmgJoystick.h"
#include <cmgInput/cmgInputManager.h>
#include <Xinput.h>


//-----------------------------------------------------------------------------
// Private Methods
//-----------------------------------------------------------------------------

Joystick::Joystick(InputManager* inputManager)
	: m_windowHandle(inputManager->GetWindowHandle())
	, m_directInput(inputManager->GetDirectInput())
	, m_joystick(nullptr)
{
	//DoInitialize();

	//DWORD dwResult;
	//for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	//{
	//	XINPUT_STATE state;
	//	ZeroMemory(&state, sizeof(XINPUT_STATE));

	//	// Simply get the state of the controller from XInput.
	//	dwResult = XInputGetState(i, &state);

	//	if (dwResult == ERROR_SUCCESS)
	//	{
	//		// Controller is connected
	//		printf("XINPUT CONTROLLER CONNECTED\n");
	//	}
	//	else
	//	{
	//		// Controller is not connected 
	//		printf("XINPUT CONTROLLER NOT CONNECTED !!!!!\n");
	//	}
	//}
}

Joystick::~Joystick()
{
	//if (m_joystick != nullptr)
	//{
	//	m_joystick->Unacquire();
	//	m_joystick->Release();
	//	m_joystick = nullptr;
	//}
}


//-----------------------------------------------------------------------------
// Input Update
//-----------------------------------------------------------------------------

void Joystick::Reset()
{
}

void Joystick::Update()
{
	m_xboxStatePrev = m_xboxState;

	//DIJOYSTATE2 state;

	//HRESULT result = m_joystick->Poll();

	//if (FAILED(result))
	//{
	//	// DirectInput is telling us that the input stream has been
	//	// interrupted. We aren't tracking any state between polls, so
	//	// we don't have any special reset that needs to be done. We
	//	// just re-acquire and try again.
	//	result = m_joystick->Acquire();
	//	while (result == DIERR_INPUTLOST)
	//	{
	//		result = m_joystick->Acquire();
	//	}

	//	// If we encounter a fatal error, return failure.
	//	if ((result == DIERR_INVALIDPARAM) || (result == DIERR_NOTINITIALIZED))
	//	{
	//		return;
	//	}

	//	// If another application has control of this device, return successfully.
	//	// We'll just have to wait our turn to use the joystick.
	//	if (result == DIERR_OTHERAPPHASPRIO)
	//	{
	//		return;
	//	}
	//}

	//// Get the input's device state
	//result = m_joystick->GetDeviceState(sizeof(DIJOYSTATE2), &state);

	//if (FAILED(result))
	//{
	//	// The device should have been acquired during the Poll()
	//	return;
	//}
	
	XINPUT_STATE xstate;
	memset(&xstate, 0, sizeof(XINPUT_STATE));
	// Simply get the state of the controller from XInput.
	DWORD dwResult = XInputGetState(0, &xstate);

	m_xboxState.a = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_A) > 0;
	m_xboxState.b = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_B) > 0;
	m_xboxState.x = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_X) > 0;
	m_xboxState.y = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_Y) > 0;
	m_xboxState.leftShoulder = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) > 0;
	m_xboxState.rightShoulder = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) > 0;
	m_xboxState.leftThumb = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) > 0;
	m_xboxState.rightThumb = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) > 0;
	m_xboxState.back = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) > 0;
	m_xboxState.start = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_START) > 0;
	m_xboxState.dpad.up = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) > 0;
	m_xboxState.dpad.down = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) > 0;
	m_xboxState.dpad.left = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) > 0;
	m_xboxState.dpad.right = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) > 0;
	m_xboxState.leftStick.x = fmaxf(-1, (float) xstate.Gamepad.sThumbLX / 32767);
	m_xboxState.leftStick.y = fmaxf(-1, (float) xstate.Gamepad.sThumbLY / 32767);
	m_xboxState.rightStick.x = fmaxf(-1, (float) xstate.Gamepad.sThumbRX / 32767);
	m_xboxState.rightStick.y = fmaxf(-1, (float) xstate.Gamepad.sThumbRY / 32767);
	m_xboxState.leftTrigger = (float) xstate.Gamepad.bLeftTrigger / 255;
	m_xboxState.rightTrigger = (float) xstate.Gamepad.bRightTrigger / 255;

	using namespace std;
	cout << "Left Stick = " << m_xboxState.leftStick << endl;
	cout << "Right Stick = " << m_xboxState.rightStick << endl;
	cout << "Right Trigger = " << m_xboxState.rightTrigger << endl;
	cout << "A = " << (m_xboxState.a ? "true" : "false") << endl;
	cout << "B = " << (m_xboxState.b ? "true" : "false") << endl;
}


//-----------------------------------------------------------------------------
// Private Methods
//-----------------------------------------------------------------------------

void Joystick::DoInitialize()
{
	// Initialize the direct input interface for the joystick
	HRESULT result = m_directInput->CreateDevice(
		GUID_SysKeyboard, &m_joystick, nullptr);
	if (FAILED(result))
	{
		fprintf(stderr, "Failure to initialize joystick device.");
		return;
	}

	// Set the data format. In this case since it is a joystick we can use the
	// predefined data format.
	result = m_joystick->SetDataFormat(&c_dfDIJoystick2);
	if (FAILED(result))
	{
		fprintf(stderr, "Could not set joystick device format.");
		return;
	}

	// Set the cooperative level of the joystick to not share with other
	// programs
	result = m_joystick->SetCooperativeLevel(
		m_windowHandle, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		fprintf(stderr, "Joystick device cooperative level settings error.");
		return;
	}

	// Determine how many axis the joystick has (so we don't error out setting
	// properties for unavailable axis)
	DIDEVCAPS capabilities;
	capabilities.dwSize = sizeof(DIDEVCAPS);
	result = m_joystick->GetCapabilities(&capabilities);
	if (FAILED(result))
	{
		fprintf(stderr, "Failure to get joystick device capabilities.");
		return;
	}
	printf("JOYSTICK = %d axes, %d buttons\n", capabilities.dwAxes, capabilities.dwButtons);

	// Now acquire the joystick
	result = m_joystick->Acquire();
	if (FAILED(result))
	{
		fprintf(stderr, "Failure to acquire joystick device.");
		return;
	}
}

