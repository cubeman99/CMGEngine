#ifndef _CMG_INPUT_MANAGER_H_
#define _CMG_INPUT_MANAGER_H_

#include <cmgCore/cmg_core.h>
#include <cmgInput/cmgInputDevice.h>
#include <vector>
#include <Windows.h>
#include <cmgInput/cmgDirectInputIncludes.h>

class Window;


// TODO: Make InputDeviceInfo NOT use void*
struct InputDeviceInfo
{
	bool			m_inUse;
	GUID			m_productGuid;
	GUID			m_deviceGuid;
	std::string		m_deviceName;
	void*			m_devicePtr;
};
	

//-----------------------------------------------------------------------------
// InputManager
//-----------------------------------------------------------------------------
class InputManager
{
public:
	typedef InputDeviceType::value_type device_type;

public:
	InputManager();
	~InputManager();

	bool Initialize(Window* window);
	void Reset(device_type type);
	void Update(device_type type);
		
	void ResetAll();
	void UpdateAll();

	InputDevice* AddDevice(device_type type);
	InputDevice* GetDevice(device_type type, int index = 0);
	int GetDeviceCount(device_type type);
		
	template <typename T_InputDevice>
	T_InputDevice* AddDevice();

	template <typename T_InputDevice>
	T_InputDevice* GetDevice(int index = 0);

	template <typename T_InputDevice>
	int GetDeviceCount();



private:
	static BOOL CALLBACK	DoEnumerateCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
	void					DoEnumerateDevices();
		
	IDirectInput8*			m_directInput;
	HWND					m_windowHandle;

	std::vector<InputDeviceInfo> m_winInputDevices[InputDeviceType::k_count];

	HWND GetWindowHandle() const { return m_windowHandle; }
	IDirectInput8* GetDirectInput() const { return m_directInput; }

	friend class Keyboard;
	friend class Mouse;
	friend class Joystick;
};


#endif // _CMG_INPUT_MANAGER_H_