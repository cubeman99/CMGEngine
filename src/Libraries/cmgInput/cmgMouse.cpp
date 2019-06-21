#include "cmgMouse.h"
#include <cmgInput/cmgInputManager.h>
#include <stdio.h>
	

//-----------------------------------------------------------------------------
// Constructor & destructor
//-----------------------------------------------------------------------------

Mouse::Mouse(InputManager* inputManager) :
	m_windowHandle(inputManager->GetWindowHandle()),
	m_directInput(inputManager->GetDirectInput()),
	m_mouse(CMG_NULL)
{
	DoInitialize();
}
	
Mouse::~Mouse()
{
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = CMG_NULL;
	}
}


//-----------------------------------------------------------------------------
// Input Update
//-----------------------------------------------------------------------------
	
void Mouse::Reset()
{
	m_previousState = m_currentState;
	m_currentState.buttons = 0;
}
	
void Mouse::Update()
{
	m_previousState = m_currentState;
	m_currentState.buttons = 0;

	// Get the mouse's device state.
	HRESULT hRes = m_mouse->GetDeviceState(s_mouseBufferSize, &m_mouseBuffer);

	// Try to reacquire the mouse.
	if (hRes == DIERR_INPUTLOST || hRes == DIERR_NOTACQUIRED)
	{
		hRes = m_mouse->Acquire();
		if (!FAILED(hRes))
		{
			hRes = m_mouse->GetDeviceState(s_mouseBufferSize, &m_mouseBuffer);
		}
	}

	if (FAILED(hRes))
		return;

	// Get the mouse X,Y location.
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(m_windowHandle, &point);
	m_currentState.x = point.x;
	m_currentState.y = point.y;

	// Add to the mouse's Z-position (scroll wheel value)
	m_currentState.z += m_mouseBuffer.lZ / 120; // TODO: magic number 120 ?

	// Poll the mouse buttons.
	if (m_mouseBuffer.rgbButtons[0] & 0x80)
		m_currentState.buttons |= MouseButtons::left;
	if (m_mouseBuffer.rgbButtons[1] & 0x80)
		m_currentState.buttons |= MouseButtons::right;
	if (m_mouseBuffer.rgbButtons[2] & 0x80)
		m_currentState.buttons |= MouseButtons::middle;
	if (m_mouseBuffer.rgbButtons[3] & 0x80)
		m_currentState.buttons |= MouseButtons::button4;
	if (m_mouseBuffer.rgbButtons[4] & 0x80)
		m_currentState.buttons |= MouseButtons::button5;
	if (m_mouseBuffer.rgbButtons[5] & 0x80)
		m_currentState.buttons |= MouseButtons::button6;
	if (m_mouseBuffer.rgbButtons[6] & 0x80)
		m_currentState.buttons |= MouseButtons::button7;
	if (m_mouseBuffer.rgbButtons[7] & 0x80)
		m_currentState.buttons |= MouseButtons::button8;
}
	

//-----------------------------------------------------------------------------
// Mouse state checks
//-----------------------------------------------------------------------------
	
bool Mouse::IsButtonDown(MouseButtons::value_type button) const
{
	return (m_currentState.buttons & button) != 0;
}
	
bool Mouse::IsButtonPressed(MouseButtons::value_type button) const
{
	return (m_currentState.buttons & button) != 0 &&
		!(m_previousState.buttons & button) != 0;
}
	
bool Mouse::IsButtonReleased(MouseButtons::value_type button) const
{
	return !(m_currentState.buttons & button) != 0 &&
		(m_previousState.buttons & button) != 0;
}


//-----------------------------------------------------------------------------
// Private Methods
//-----------------------------------------------------------------------------

void Mouse::DoInitialize()
{
	// Initialize the direct input interface for the mouse.
	HRESULT result = m_directInput->CreateDevice(
		GUID_SysMouse, &m_mouse, CMG_NULL);
	if (FAILED(result))
	{
		fprintf(stderr, "Failure to initialize mouse.\n");
		return;
	}

	// Set the cooperative level of the mouse to not share with other programs.
	result = m_mouse->SetCooperativeLevel(
		m_windowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);//DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		fprintf(stderr, "Mouse cooperative level settings error.\n");
		return;
	}

	// Set the data format using the predefined format for DIMOUSESTATE2;
	result = m_mouse->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(result))
	{
		fprintf(stderr, "Could not set mouse device format.\n");
		return;
	}

	// Acquire the mouse.
	result = m_mouse->Acquire();
	if (FAILED(result) && result != DIERR_OTHERAPPHASPRIO)
	{
		fprintf(stderr, "Failure to acquire mouse.\n");
		return;
	}
}


