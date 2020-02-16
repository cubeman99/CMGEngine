#include "cmgInputManager.h"
#include <cmgCore/cmg_core.h>
#include <cmgGraphics/cmgWindow.h>
#include <cmgInput/cmgMouse.h>	
#include <cmgInput/cmgKeyboard.h>
#include <cmgInput/cmgJoystick.h>


#define INPUT_MANAGER_IMPL_TEMP   typename T_ParentInputManager
#define INPUT_MANAGER_IMPL_PARAM  T_ParentInputManager

#define ASSERT_VALID_DEVICE_TYPE(_deviceType) \
	CMG_ASSERT_MSG((int32) _deviceType >= 0 && \
		(int32) _deviceType < (int32) InputDeviceType::k_count, \
		"Invalid device type.")




	
void InputManager::ResetAll()
{
	for (int32 i = 0; i < (int32) InputDeviceType::k_count; i++)
		Reset((InputDeviceType) i);
}

void InputManager::UpdateAll()
{
	for (int32 i = 0; i < (int32) InputDeviceType::k_count; i++)
		Update((InputDeviceType) i);
}
	
template <typename T_InputDevice>
T_InputDevice* InputManager::AddDevice()
{
	return (T_InputDevice*) AddDevice(T_InputDevice::k_device_type);
}

template<typename T_InputDevice>
T_InputDevice* InputManager::AddDevice(const String& name)
{
	return (T_InputDevice*) AddDevice(T_InputDevice::k_device_type, name);
}
	
template <typename T_InputDevice>
T_InputDevice* InputManager::GetDevice(int index)
{
	return (T_InputDevice*) GetDevice(T_InputDevice::k_device_type, index);
}

template<typename T_InputDevice>
T_InputDevice* InputManager::GetDevice(const String& name)
{
	return (T_InputDevice*) GetDevice(T_InputDevice::k_device_type, name);
}
	
template <typename T_InputDevice>
int InputManager::GetDeviceCount()
{
	return GetDeviceCount(T_InputDevice::k_device_type);
}






InputManager::InputManager() :
	m_directInput(CMG_NULL)
{
}

InputManager::~InputManager()
{
	// Delete all devices.
	for (int i = 0; i < (int32) InputDeviceType::k_count; i++)
	{
		for (auto it = m_winInputDevices[i].begin(); it !=  m_winInputDevices[i].end(); ++it)
		{
			if (it->m_inUse)
			{
				delete it->m_devicePtr;
				it->m_devicePtr = CMG_NULL;
			}
		}
	}

	// Release the direct input context.
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = CMG_NULL;
	}
}
		
bool InputManager::Initialize(Window* window)
{
	CMG_ASSERT(window != nullptr);

	m_windowHandle = window->GetWindowHandle();

	HINSTANCE hInst = CMG_NULL;
	HRESULT hr;

	if (IsWindow(m_windowHandle) == 0)
	{
		//TLOC_LOG_INPUT_ERR() << "The passed window handle is not valid";
		CMG_ASSERT_FALSE("The passed window handle is not valid");
		return false;
	}

	hInst = GetModuleHandle(CMG_NULL);

	// Create the input device.
	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(VOID**) &m_directInput, CMG_NULL);
	if (FAILED(hr))
	{
		//TLOC_LOG_INPUT_ERR() << "Unable to initialize direct input";
		CMG_ASSERT_FALSE("Unable to initialize direct input");
		return false;
	}

	DoEnumerateDevices();
	return true;
}

void InputManager::Reset(InputDeviceType type)
{
	ASSERT_VALID_DEVICE_TYPE(type);

	for (size_t i = 0; i < m_winInputDevices[(int32) type].size(); ++i)
	{
		if (m_winInputDevices[(int32) type][i].m_devicePtr != CMG_NULL)
		{
			InputDevice* devicePtr = (InputDevice*) m_winInputDevices[(int32) type][i].m_devicePtr;
			devicePtr->Reset();
		}
	}
}

void InputManager::Update(InputDeviceType type)
{
	ASSERT_VALID_DEVICE_TYPE(type);

	for (size_t i = 0; i < m_winInputDevices[(int32) type].size(); ++i)
	{
		if (m_winInputDevices[(int32) type][i].m_devicePtr != CMG_NULL)
		{
			InputDevice* devicePtr = (InputDevice*) m_winInputDevices[(int32) type][i].m_devicePtr;
			devicePtr->Update();
		}
	}
}

InputDevice* InputManager::AddDevice(InputDeviceType type)
{
	ASSERT_VALID_DEVICE_TYPE(type);

	if (m_winInputDevices[(int32) type].empty())
	{
		CMG_ASSERT_FALSE("No input device found.");
		return nullptr;
	}
		
	// Find an unused device of the given type.
	for (auto& deviceInfo : m_winInputDevices[(int32) type])
	{
		if (!deviceInfo.m_inUse)
			return CreateDevice(type, deviceInfo);
	}
		
	CMG_ASSERT_FALSE("All input devices are in use.");
	return CMG_NULL;
}

InputDevice* InputManager::AddDevice(InputDeviceType type, const String & name)
{
	ASSERT_VALID_DEVICE_TYPE(type);

	for (auto& deviceInfo : m_winInputDevices[(int32) type])
	{
		if (deviceInfo.m_deviceName == name)
		{
			if (!deviceInfo.m_inUse)
				return CreateDevice(type, deviceInfo);
			else
				CMG_ASSERT_FALSE("Input device is already in use!");
		}
	}

	return CMG_NULL;
}

InputDevice* InputManager::GetDevice(InputDeviceType type, int index)
{
	ASSERT_VALID_DEVICE_TYPE(type);
	CMG_ASSERT_MSG(index < (int) m_winInputDevices[(int32)type].size(), "Index out of range!");
	return (InputDevice*) m_winInputDevices[(int32)type][index].m_devicePtr;
}

InputDevice* InputManager::GetDevice(InputDeviceType type, const String& name)
{
	ASSERT_VALID_DEVICE_TYPE(type);
	for (auto device : m_winInputDevices[(int32) type])
	{
		if (device.m_deviceName == name)
		{
			return (InputDevice*) device.m_devicePtr;
		}
	}
	return nullptr;
}

int InputManager::GetDeviceCount(InputDeviceType type)
{
	return (int32) m_winInputDevices[(int32) type].size();
}

	
//-----------------------------------------------------------------------------
// Private Methods
//-----------------------------------------------------------------------------

void InputManager::DoEnumerateDevices()
{
	// Enumerate input devices with our callback.
	m_directInput->EnumDevices(CMG_NULL, &InputManager::DoEnumerateCallback,
		this, DIEDFL_ATTACHEDONLY);

	// Now add other devices that are dummies such as touch
	InputDeviceInfo dummyInfo;
	dummyInfo.m_productGuid	= GUID();
	dummyInfo.m_deviceGuid	= GUID();
	dummyInfo.m_deviceName	= "DummyDevice";
	dummyInfo.m_inUse		= false;
	dummyInfo.m_devicePtr	= CMG_NULL;
		
	m_winInputDevices[(int32)InputDeviceType::k_touch_surface].push_back(dummyInfo);
}

InputDevice* InputManager::CreateDevice(InputDeviceType type, InputDeviceInfo& deviceInfo)
{
	// Create the correct input device.
	InputDevice* device = nullptr;
	if (type == InputDeviceType::k_keyboard)
		device = new Keyboard(this);
	else if (type == InputDeviceType::k_mouse)
		device = new Mouse(this);
	else if (type == InputDeviceType::k_joystick)
		device = new Joystick(this, deviceInfo);

	if (device != nullptr)
	{
		printf("Added input device: %s\n", deviceInfo.m_deviceName.c_str());
	}

	deviceInfo.m_inUse = true;
	deviceInfo.m_devicePtr = device;
	return device;
}

BOOL InputManager::DoEnumerateCallback(LPCDIDEVICEINSTANCE instance, LPVOID pvRef)
{
	InputManager* inputMgr = static_cast<InputManager*>(pvRef);

	InputDeviceInfo info;
	info.m_productGuid	= instance->guidProduct;
	info.m_deviceGuid	= instance->guidInstance;
	info.m_deviceName	= instance->tszInstanceName;
	info.m_inUse		= false;
	info.m_devicePtr	= CMG_NULL;
	uint32 deviceType = GET_DIDEVICE_TYPE(instance->dwDevType);
	String typeName = "UNKNOWN";
	if (deviceType == DI8DEVTYPE_KEYBOARD)
		typeName = "Keyboard";
	else if (deviceType == DI8DEVTYPE_MOUSE)
		typeName = "Mouse";
	else if (deviceType == DI8DEVTYPE_DRIVING)
		typeName = "Driving";
	else if (deviceType == DI8DEVTYPE_JOYSTICK)
		typeName = "Joystick";
	else if (deviceType == DI8DEVTYPE_GAMEPAD)
		typeName = "GamePad";
	else if (deviceType == DI8DEVTYPE_FLIGHT)
		typeName = "Flight";
	else if (deviceType == DI8DEVTYPE_DEVICE)
		typeName = "Device";
	else if (deviceType == DI8DEVTYPE_DEVICECTRL)
		typeName = "Device Controller";

	printf("Device: %s (%s)\n", instance->tszInstanceName, typeName.c_str());
		
	if (deviceType == DI8DEVTYPE_KEYBOARD)
	{
		inputMgr->m_winInputDevices[(int32) InputDeviceType::k_keyboard].push_back(info);
	}
	else if (deviceType == DI8DEVTYPE_MOUSE)
	{
		inputMgr->m_winInputDevices[(int32) InputDeviceType::k_mouse].push_back(info);
	}
	else if (deviceType == DI8DEVTYPE_JOYSTICK ||
		deviceType == DI8DEVTYPE_GAMEPAD ||
		deviceType == DI8DEVTYPE_1STPERSON ||
		deviceType == DI8DEVTYPE_DRIVING ||
		deviceType == DI8DEVTYPE_FLIGHT)
	{
		inputMgr->m_winInputDevices[(int32) InputDeviceType::k_joystick].push_back(info);
	}

	return DIENUM_CONTINUE;
}



//-----------------------------------------------------------------------------
// Explicit Instantiations
//-----------------------------------------------------------------------------

template Keyboard*	InputManager::AddDevice<Keyboard>();
template Mouse*		InputManager::AddDevice<Mouse>();
template Joystick*	InputManager::AddDevice<Joystick>();

template Keyboard*	InputManager::AddDevice<Keyboard>(const String& name);
template Mouse*		InputManager::AddDevice<Mouse>(const String& name);
template Joystick*	InputManager::AddDevice<Joystick>(const String& name);

template Keyboard*	InputManager::GetDevice<Keyboard>(int index);
template Mouse*		InputManager::GetDevice<Mouse>(int index);
template Joystick*	InputManager::GetDevice<Joystick>(int index);

template Keyboard*	InputManager::GetDevice<Keyboard>(const String& index);
template Mouse*		InputManager::GetDevice<Mouse>(const String& index);
template Joystick*	InputManager::GetDevice<Joystick>(const String& index);
	
template int InputManager::GetDeviceCount<Keyboard>();
template int InputManager::GetDeviceCount<Mouse>();
template int InputManager::GetDeviceCount<Joystick>();

