#ifndef _CMG_INPUT_DEVICE_H_
#define _CMG_INPUT_DEVICE_H_

#include <cmgCore/cmgBase.h>

class InputManager;


//-----------------------------------------------------------------------------
// Enum for device types.
//-----------------------------------------------------------------------------
enum class InputDeviceType
{
	k_keyboard = 0,
	k_mouse,
	k_joystick,
	k_touch_surface,
	k_count
};
	

//-----------------------------------------------------------------------------
// Base class for input devices.
//-----------------------------------------------------------------------------
class InputDevice
{
public:
	InputDevice() {}
	virtual ~InputDevice() {}
		
	virtual void Reset() = 0;
	virtual void Update() = 0;

protected:
	InputManager* m_inputManager;
};


#endif // _CMG_INPUT_DEVICE_H_