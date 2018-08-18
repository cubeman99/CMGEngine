#include "cmgJoystick.h"
#include <cmgInput/cmgInputManager.h>
#include <cmgMath/cmgMathLib.h>
#include <Xinput.h>


//-----------------------------------------------------------------------------
// Private Methods
//-----------------------------------------------------------------------------

Joystick::Joystick(InputManager* inputManager, const InputDeviceInfo& deviceInfo)
	: m_windowHandle(inputManager->GetWindowHandle())
	, m_directInput(inputManager->GetDirectInput())
	, m_joystick(nullptr)
{
	DoInitialize(deviceInfo);
}

Joystick::~Joystick()
{
	if (m_joystick != nullptr)
	{
		m_joystick->Unacquire();
		m_joystick->Release();
		m_joystick = nullptr;
	}
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

	DIJOYSTATE2 state;

	HRESULT result = m_joystick->Poll();

	if (FAILED(result))
	{
		// DirectInput is telling us that the input stream has been
		// interrupted. We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done. We
		// just re-acquire and try again.
		result = m_joystick->Acquire();
		while (result == DIERR_INPUTLOST)
		{
			result = m_joystick->Acquire();
		}

		// If we encounter a fatal error, return failure.
		if ((result == DIERR_INVALIDPARAM) || (result == DIERR_NOTINITIALIZED))
		{
			return;
		}

		// If another application has control of this device, return successfully.
		// We'll just have to wait our turn to use the joystick.
		if (result == DIERR_OTHERAPPHASPRIO)
		{
			return;
		}
	}

	// Get the input's device state
	result = m_joystick->GetDeviceState(sizeof(DIJOYSTATE2), &state);

	if (FAILED(result))
	{
		// The device should have been acquired during the Poll()
		return;
	}

	using namespace std;

	//cout << "JOYSTICK STATE:" << endl;
	for (Axis* axis : m_axes)
	{
		axis->x = (float) (&state.lX)[axis->index] / 1000.0f;
		//axis.value = 0.0f;
		//cout << "  " << axis.index << ": " << axis.x << endl;
	}
	for (Button* button : m_buttons)
	{
		button->down = (state.rgbButtons[button->index] > 0);
		m_xboxState.buttons[button->index] = button->down;

		cout << "Button" << button->index << " " << (button->down ? "TRUE" : "false") << endl;
	}
	for (POV* pov : m_povs)
	{
		DWORD value = state.rgdwPOV[pov->index];
		if (LOWORD(value) == 0xFFFF)
		{
			pov->angle = 0.0f;
			pov->centered = true;
		}
		else
		{
			pov->centered = false;
			pov->angle = ((float) state.rgdwPOV[0] / (float) 36000) * Math::TWO_PI;
		}
	}

	// steering = state.lX
	// right pedal = state.lY
	// left pedal = state.lRz

	ZeroMemory(&m_xboxState, sizeof(XboxControllerState));
	m_xboxState.leftStick.x = m_axisPool[0].x;
	m_xboxState.leftStick.y = m_axisPool[1].x;
	m_xboxState.leftTrigger = m_axisPool[2].x;
	m_xboxState.rightTrigger = -m_axisPool[2].x;
	m_xboxState.rightStick.x = m_axisPool[3].x;
	m_xboxState.rightStick.y = m_axisPool[4].x;

	//uint32 line = 1;
	//cout << line++ << ": " << ", " << state.lX << endl;
	//cout << line++ << ": " << ", " << state.lY << endl;
	//cout << line++ << ": " << ", " << state.lZ << endl;
	//cout << line++ << ": " << ", " << state.lRx << endl;
	//cout << line++ << ": " << ", " << state.lRy << endl;
	//cout << line++ << ": " << ", " << state.lRz << endl;
	//cout << line++ << ": " << ", " << state.rglSlider[0] << endl;
	//cout << line++ << ": " << ", " << state.rglSlider[1] << endl;
	//cout << line++ << ": " << ", " << state.rgdwPOV[0] << endl;
	//cout << line++ << ": " << ", " << state.rgdwPOV[1] << endl;
	//cout << line++ << ": " << ", " << state.rgdwPOV[2] << endl;
	//cout << line++ << ": " << ", " << state.rgdwPOV[3] << endl;

	//cout << "Left Stick = " << m_xboxState.leftStick << endl;
	//cout << "Right Stick = " << m_xboxState.rightStick << endl;
	//cout << "Right Trigger = " << m_xboxState.rightTrigger << endl;
	//cout << "A = " << (m_xboxState.a ? "true" : "false") << endl;
	//cout << "B = " << (m_xboxState.b ? "true" : "false") << endl;

	//XINPUT_STATE xstate;
	//memset(&xstate, 0, sizeof(XINPUT_STATE));
	//DWORD dwResult = XInputGetState(0, &xstate);

	//m_xboxState.a = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_A) > 0;
	//m_xboxState.b = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_B) > 0;
	//m_xboxState.x = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_X) > 0;
	//m_xboxState.y = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_Y) > 0;
	//m_xboxState.leftShoulder = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) > 0;
	//m_xboxState.rightShoulder = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) > 0;
	//m_xboxState.leftThumb = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) > 0;
	//m_xboxState.rightThumb = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) > 0;
	//m_xboxState.back = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) > 0;
	//m_xboxState.start = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_START) > 0;
	//m_xboxState.dpad.up = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) > 0;
	//m_xboxState.dpad.down = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) > 0;
	//m_xboxState.dpad.left = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) > 0;
	//m_xboxState.dpad.right = (xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) > 0;
	//m_xboxState.leftStick.x = fmaxf(-1, (float) xstate.Gamepad.sThumbLX / 32767);
	//m_xboxState.leftStick.y = fmaxf(-1, (float) xstate.Gamepad.sThumbLY / 32767);
	//m_xboxState.rightStick.x = fmaxf(-1, (float) xstate.Gamepad.sThumbRX / 32767);
	//m_xboxState.rightStick.y = fmaxf(-1, (float) xstate.Gamepad.sThumbRY / 32767);
	//m_xboxState.leftTrigger = (float) xstate.Gamepad.bLeftTrigger / 255;
	//m_xboxState.rightTrigger = (float) xstate.Gamepad.bRightTrigger / 255;

	//using namespace std;
	//cout << "Left Stick = " << m_xboxState.leftStick << endl;
	//cout << "Right Stick = " << m_xboxState.rightStick << endl;
	//cout << "Right Trigger = " << m_xboxState.rightTrigger << endl;
	//cout << "A = " << (m_xboxState.a ? "true" : "false") << endl;
	//cout << "B = " << (m_xboxState.b ? "true" : "false") << endl;
}


//-----------------------------------------------------------------------------
// Private Methods
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: EnumObjectsCallback()
// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a 
//       joystick. This function enables user interface elements for objects
//       that are found to exist, and scales axes min/max values.
//-----------------------------------------------------------------------------
BOOL CALLBACK Joystick::EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* instance, void* context)
{
	Joystick* joystick = (Joystick*) context;
	static int nSliderCount = 0;  // Number of returned slider controls
	static int nPOVCount = 0;     // Number of returned POV controls

	uint32 type = DIDFT_GETTYPE(instance->dwType);
	uint32 instanceNumber = DIDFT_GETINSTANCE(instance->dwType);
	printf(" - %u. TYPE = %04X, FLAGS = %X, NAME = %s\n", instanceNumber, type, instance->dwFlags, instance->tszName);

	// For axes that are returned, set the DIPROP_RANGE property for the
	// enumerated axis in order to scale min/max values.
	if (type & DIDFT_AXIS)
	{
		DIPROPRANGE diprg;
		diprg.diph.dwSize = sizeof(DIPROPRANGE);
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		diprg.diph.dwHow = DIPH_BYID;
		diprg.diph.dwObj = instance->dwType; // Specify the enumerated axis
		diprg.lMin = -1000;
		diprg.lMax = +1000;

		// Set the range for the axis
		HRESULT result = joystick->m_joystick->SetProperty(DIPROP_RANGE, &diprg.diph);
		if (FAILED(result))
			return DIENUM_STOP;

		//printf(" - AXIS DETECTED: %u, %s\n", instance->dwType, instance->tszName);

		Axis* axis = joystick->m_axisPool + instanceNumber;
		axis->xyz = Vector3f::ZERO;
		axis->index = instanceNumber;
		joystick->m_axes.push_back(axis);
	}
	else if (type & DIDFT_BUTTON)
	{
		Button* button = joystick->m_buttonPool + instanceNumber;
		button->down = false;
		button->index = instanceNumber;
		joystick->m_buttons.push_back(button);
		//printf(" - BUTTON DETECTED: %u, %s\n", instance->dwType, instance->tszName);
	}
	else if (type & DIDFT_POV)
	{
		POV* pov = joystick->m_povPool + instanceNumber;
		pov->angle = 0.0f;
		pov->centered = true;
		pov->index = instanceNumber;
		joystick->m_povs.push_back(pov);
		//printf(" - POV DETECTED: %u, %s\n", instance->dwType, instance->tszName);
	}
	//if (instance->dwFlags & DIDFT_FFACTUATOR || instance->dwType & DIDOI_FFEFFECTTRIGGER)
	//{
		//printf("   - max force = %u N\n", instance->dwFFMaxForce);
		//printf("   - max force = %u N\n", instance->dwFFForceResolution);
	//}
	//if (instance->dwType & DIDFT_COLLECTION)
	//printf(" - COLLECTION DETECTED: %s\n", instance->tszName);


	//// Set the UI to reflect what objects the joystick supports
	//if( pdidoi->guidType == GUID_XAxis )
	//{
	//    EnableWindow( GetDlgItem( hDlg, IDC_X_AXIS ), TRUE );
	//    EnableWindow( GetDlgItem( hDlg, IDC_X_AXIS_TEXT ), TRUE );
	//}
	//if( pdidoi->guidType == GUID_YAxis )
	//{
	//    EnableWindow( GetDlgItem( hDlg, IDC_Y_AXIS ), TRUE );
	//    EnableWindow( GetDlgItem( hDlg, IDC_Y_AXIS_TEXT ), TRUE );
	//}
	//if( pdidoi->guidType == GUID_ZAxis )
	//{
	//    EnableWindow( GetDlgItem( hDlg, IDC_Z_AXIS ), TRUE );
	//    EnableWindow( GetDlgItem( hDlg, IDC_Z_AXIS_TEXT ), TRUE );
	//}
	//if( pdidoi->guidType == GUID_RxAxis )
	//{
	//    EnableWindow( GetDlgItem( hDlg, IDC_X_ROT ), TRUE );
	//    EnableWindow( GetDlgItem( hDlg, IDC_X_ROT_TEXT ), TRUE );
	//}
	//if( pdidoi->guidType == GUID_RyAxis )
	//{
	//    EnableWindow( GetDlgItem( hDlg, IDC_Y_ROT ), TRUE );
	//    EnableWindow( GetDlgItem( hDlg, IDC_Y_ROT_TEXT ), TRUE );
	//}
	//if( pdidoi->guidType == GUID_RzAxis )
	//{
	//    EnableWindow( GetDlgItem( hDlg, IDC_Z_ROT ), TRUE );
	//    EnableWindow( GetDlgItem( hDlg, IDC_Z_ROT_TEXT ), TRUE );
	//}
	//if( pdidoi->guidType == GUID_Slider )
	//{
	//    switch( nSliderCount++ )
	//    {
	//        case 0 :
	//            EnableWindow( GetDlgItem( hDlg, IDC_SLIDER0 ), TRUE );
	//            EnableWindow( GetDlgItem( hDlg, IDC_SLIDER0_TEXT ), TRUE );
	//            break;

	//        case 1 :
	//            EnableWindow( GetDlgItem( hDlg, IDC_SLIDER1 ), TRUE );
	//            EnableWindow( GetDlgItem( hDlg, IDC_SLIDER1_TEXT ), TRUE );
	//            break;
	//    }
	//}
	//if( pdidoi->guidType == GUID_POV )
	//{
	//    switch( nPOVCount++ )
	//    {
	//        case 0 :
	//            EnableWindow( GetDlgItem( hDlg, IDC_POV0 ), TRUE );
	//            EnableWindow( GetDlgItem( hDlg, IDC_POV0_TEXT ), TRUE );
	//            break;

	//        case 1 :
	//            EnableWindow( GetDlgItem( hDlg, IDC_POV1 ), TRUE );
	//            EnableWindow( GetDlgItem( hDlg, IDC_POV1_TEXT ), TRUE );
	//            break;

	//        case 2 :
	//            EnableWindow( GetDlgItem( hDlg, IDC_POV2 ), TRUE );
	//            EnableWindow( GetDlgItem( hDlg, IDC_POV2_TEXT ), TRUE );
	//            break;

	//        case 3 :
	//            EnableWindow( GetDlgItem( hDlg, IDC_POV3 ), TRUE );
	//            EnableWindow( GetDlgItem( hDlg, IDC_POV3_TEXT ), TRUE );
	//            break;
	//    }
	//}

	return DIENUM_CONTINUE;
}

void Joystick::DoInitialize(const InputDeviceInfo& deviceInfo)
{
	// Initialize the direct input interface for the joystick
	HRESULT result = m_directInput->CreateDevice(
		deviceInfo.m_deviceGuid, &m_joystick, nullptr);
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
	printf("JOYSTICK = %u axes, %u buttons\n", capabilities.dwAxes, capabilities.dwButtons);


	// values property for discovered axes
	result = m_joystick->EnumObjects(
		Joystick::EnumObjectsCallback, (void*) this, DIDFT_ALL);
	if (FAILED(result))
	{
		fprintf(stderr, "Failure to enumerate joystick objects.");
		return;
	}

	// Now acquire the joystick
	result = m_joystick->Acquire();
	if (FAILED(result))
	{
		fprintf(stderr, "Failure to acquire joystick device.");
		return;
	}
}

