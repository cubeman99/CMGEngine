#include "cmgWindow.h"
#include <cmgCore/cmgAssert.h>
#include <cmgGraphics/cmgOpenGLIncludes.h>
#include <iostream>

	
#define VERIFY_DEVICE_CONTEXT() \
    CMG_ASSERT_MSG(m_deviceContext, "No device context available -- did you call "\
       "Create()?");
#define VERIFY_OPENGL_CONTEXT() \
    CMG_ASSERT_MSG(m_openGLContext, "No OpenGL context available -- did you call "\
       "Create()? -- If yes, it may not have succeeded");



const wchar_t	g_className[]			= L"CMG_Window";
Window*			g_currFullScreenWindow	= nullptr;
size_t			g_currWindowCount		= 0;
Window*			g_currentActiveWindow	= nullptr;


namespace
{
	// Get the error string for the given HRESULT value.
	// GetLastError() can be used to get that latest HRESULT.
	String GetHResultErrorString(HRESULT hResult)
	{
		LPTSTR errorText = NULL;

		// Convoluted error string retrieval: http://stackoverflow.com/a/455533/368599
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM // use system message tables to retrieve error text
			| FORMAT_MESSAGE_ALLOCATE_BUFFER // allocate text buffer on local heap
			| FORMAT_MESSAGE_IGNORE_INSERTS, // Important! will fail otherwise, since we're not (and CANNOT) pass insertion parameters
			NULL, // unused with FORMAT_MESSAGE_FROM_SYSTEM
			hResult,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &errorText,  // output
			0, // minimum size for output buffer
			NULL); // arguments - see note

		String text = errorText;
		LocalFree(errorText);
		return text;
	}
};



WindowDropTarget::WindowDropTarget(Window* window) :
	m_window(window),
	m_refCount(1)
{
}

HRESULT WindowDropTarget::QueryInterface(REFIID riid, void** ppvObject)
{
    if (ppvObject == nullptr)
        return E_INVALIDARG;

	// Return a pointer to this if querying the following interfaces.
    if (riid == IID_IUnknown || riid == IID_IDropTarget)
    {
        *ppvObject = (LPVOID) this;
        AddRef();
        return NOERROR;
    }
	
	*ppvObject = nullptr;
	return E_NOINTERFACE;
}
	
HRESULT WindowDropTarget::DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	*pdwEffect = DROPEFFECT_COPY | DROPEFFECT_LINK;
	return S_OK;
}

HRESULT WindowDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	*pdwEffect = DROPEFFECT_COPY | DROPEFFECT_LINK;
	return S_OK;
}

HRESULT WindowDropTarget::DragLeave()
{
	return S_OK;
}

HRESULT WindowDropTarget::Drop(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	//*pdwEffect = DROPEFFECT_NONE;
	*pdwEffect = DROPEFFECT_COPY | DROPEFFECT_LINK;
	
	FORMATETC format = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM medium;

    // Dropped text.
	format.cfFormat = CF_TEXT;
	if (pDataObject->GetData(&format, &medium) == S_OK)
	{
		WindowEvent winEvent(WindowEvent::k_drop_text);

		// We need to lock the HGLOBAL handle because we can't
		// be sure if this is GMEM_FIXED (i.e. normal heap) data or not.
		char* data = (char*) GlobalLock(medium.hGlobal);
		winEvent.drop.text = data;
		GlobalUnlock(medium.hGlobal);
		GlobalFree(medium.hGlobal);

		m_window->SendEvent(winEvent);
	}
	
    // Dropped files (HDROP)
	format.cfFormat = CF_HDROP;
    if (pDataObject->GetData(&format, &medium) == S_OK)
	{
		WindowEvent winEvent(WindowEvent::k_drop_file);
		char fileName[MAX_PATH];

		DragQueryFile((HDROP) medium.hGlobal, 0, fileName, sizeof (fileName));
		winEvent.drop.text = fileName;
		if (medium.pUnkForRelease != nullptr)
			medium.pUnkForRelease->Release();
		else
			GlobalFree(medium.hGlobal);
		
		m_window->SendEvent(winEvent);
	}


	return S_OK;
}



//-----------------------------------------------------------------------------
// Constructors and destructor
//-----------------------------------------------------------------------------

Window::Window() :
	m_handle(nullptr),
	m_callbackPtr(0),
	m_cursor(nullptr),
	m_icon(nullptr),
	m_isCursorIn(false),
	m_deviceContext(nullptr),
	m_openGLContext(nullptr),
	m_dropTarget(this),
	m_mouseVisible(true),
	m_mouseConfined(false)
{
}

Window::~Window()
{
	Close();
}
	
void Window::Close()
{
	if (m_icon)
		DestroyIcon(m_icon);

	if (m_callbackPtr == 0)
	{
		if (m_handle)
			DestroyWindow(m_handle);

		--g_currWindowCount;

		// If this is the last window...
		if (g_currWindowCount == 0)
		{
			UnregisterClassW(g_className, GetModuleHandle(nullptr));
		}
	}
	else
	{
		// Window is external, which we are not handling yet
		CMG_ASSERT_FALSE("External windows are not supported yet");
	}
}


//-----------------------------------------------------------------------------
// Window Creation
//-----------------------------------------------------------------------------
	
void Window::Create(const GraphicsMode& graphicsMode, const WindowSettings& winSettings)
{
	m_graphicsMode = graphicsMode;
	m_windowSettings = winSettings;
	m_mouseConfined = false;
	m_mouseVisible = true;
		
	// Register the window class when the first window is created.
	if (g_currWindowCount == 0)
		DoRegisterWindowClass();
		
	// Convert the title string to a wide-character string.
	const size_t wTitleSize = 256;
	wchar_t wTitle[wTitleSize];// = L"Title"; // TODO
	size_t retIndex = string::CharAsciiToWide(wTitle, winSettings.GetTitle().c_str(), wTitleSize);
	wTitle[retIndex] = L'\0';

	// Get the appropriate window styles from the window flags.
	DWORD windowStyle;
	DWORD windowExStyle;
	GetWindowStyles(winSettings.GetFlags(), windowStyle, windowExStyle);

	// Get the native resolution of the screen.
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	
	// Create a RECT for the window's position & size in WINDOWED-MODE.
	// Then adjust the rect based on the window style (a border will increase size).
	m_windowedWindowRect.left = (screenWidth - graphicsMode.GetWidth()) / 2;;
	m_windowedWindowRect.top = (screenHeight - graphicsMode.GetHeight()) / 2;;
	m_windowedWindowRect.right = m_windowedWindowRect.left + graphicsMode.GetWidth();
	m_windowedWindowRect.bottom = m_windowedWindowRect.top + graphicsMode.GetHeight();
	AdjustWindowRectEx(&m_windowedWindowRect, windowStyle, false, windowExStyle);

	// The RECT used for the window's starting position.
	RECT windowRect;
	bool isFullScreen = ((winSettings.GetFlags() & WindowFlags::k_fullscreen) != 0 ||
		(winSettings.GetFlags() & WindowFlags::k_fake_fullscreen) != 0);
	if (isFullScreen)
		windowRect = { 0, 0, screenWidth, screenHeight };
	else
		windowRect = m_windowedWindowRect;

	// Create the window and check for errors.
	m_handle = CreateWindowExW(windowExStyle, g_className, wTitle, windowStyle,
		(int) windowRect.left, (int) windowRect.top,
		(int) (windowRect.right - windowRect.left),
		(int) (windowRect.bottom - windowRect.top),
		nullptr, nullptr, GetModuleHandle(nullptr), this);
	
	if (m_handle == nullptr)
	{
		String errorText = GetHResultErrorString(GetLastError());
		printf("CreateWindowW failed: %s", errorText.c_str());
	}

	//if (isFullScreen)
		//DoSwitchToFullscreen(graphicsMode);
		
	DoCreateContext(graphicsMode, m_windowSettings);
	
	++g_currWindowCount;
		
	// (From SFML): Actual size of window may be different after the
	// AdjustWindowRect() call.
	RECT actualRect;
	GetClientRect(m_handle, &actualRect);
	m_graphicsMode.SetWidth(actualRect.right - actualRect.left);
	m_graphicsMode.SetHeight(actualRect.bottom - actualRect.top);

	GetClipCursor(&m_defaultMouseClip);

	HRESULT hresult;
	hresult = CoInitialize(nullptr);
	hresult = OleInitialize(nullptr);
	//hresult = CoLockObjectExternal(&m_dropTarget, TRUE, FALSE);
	hresult = RegisterDragDrop(m_handle, &m_dropTarget);
	if (hresult != S_OK)
		printf("Error: unable to register window as a drop target\n");
}


//-----------------------------------------------------------------------------
// Getters
//-----------------------------------------------------------------------------
	
WindowHandle::type Window::GetWindowHandle() const
{
	return m_handle;
}

int Window::GetWidth() const
{
	return m_graphicsMode.GetWidth();
}

int Window::GetHeight() const
{
	return m_graphicsMode.GetHeight();
}

float Window::GetAspectRatio() const
{
	return ((float) GetWidth() / (float) GetHeight());
}

bool Window::IsCreated() const
{
	return (IsWindow(m_handle) == TRUE);
}

bool Window::IsActive() const
{
	return (GetCurrentActiveWindow() == this);
}

bool Window::IsMouseVisible() const
{
	return m_mouseVisible;
}

bool Window::IsMouseConfined() const
{
	return m_mouseConfined;
}

bool Window::IsFullscreen() const
{
	return (m_windowSettings.GetFlags() & WindowFlags::k_fullscreen) != 0 ||
		(m_windowSettings.GetFlags() & WindowFlags::k_fake_fullscreen) != 0;
}

	
//-----------------------------------------------------------------------------
// Setters
//-----------------------------------------------------------------------------

void Window::SetActive(bool active)
{
	if (active)
	{
		if (m_deviceContext && m_openGLContext && (wglGetCurrentContext() != m_openGLContext))
		{
			Window::SetCurrentActiveWindow(this);
			wglMakeCurrent(m_deviceContext, m_openGLContext);
		}
		else
		{
			//TLOC_LOG_GFX_WARN() << "Window already active";
		}
	}
	else
	{
		if (wglGetCurrentContext() == m_openGLContext)
		{
			Window::SetNonActiveWindow(this);
			wglMakeCurrent(nullptr, nullptr);
		}
		else
		{
			//TLOC_LOG_GFX_WARN() << "Window already inactive";
		}
	}
}
	
// Set the position of the window in the screen.
void Window::SetPosition(int x, int y)
{
	SetWindowPos(m_handle, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}
	
// Set the size of the window.
void Window::SetSize(int a_width, int a_height)
{
	LONG width  = (LONG) a_width;
	LONG height = (LONG) a_height;

	// SFML: We have to adjust according to the total size of the window which
	// includes the title bar, borders etc.
	RECT rect = { 0, 0, width, height };
	AdjustWindowRect(&rect, GetWindowLong(m_handle, GWL_STYLE), false);

	width  = rect.right - rect.left;
	height = rect.bottom - rect.top;

	// The events are fired as soon as SetWindowPos is TRUE. So we set the
	// width and height of m_graphicsMode and if FALSE, we reset it.
	int oldWidth  = m_graphicsMode.GetWidth();
	int oldHeight = m_graphicsMode.GetHeight();

	m_graphicsMode.SetWidth(a_width); // TODO: this wont work for min/max sizes
	m_graphicsMode.SetHeight(a_height);

	if (SetWindowPos(m_handle, nullptr, 0, 0,
		width, height, SWP_NOMOVE | SWP_NOZORDER) == FALSE)
	{
		m_graphicsMode.SetWidth(oldWidth);
		m_graphicsMode.SetHeight(oldHeight);
	}
}

// Set the window title.
void Window::SetTitle(const String& title)
{
	m_windowSettings.SetTitle(title);
	SetWindowText(m_handle, title.c_str());
}

// Set whether vertical sync is to be used.
void Window::SetVerticalSync(bool useVSync)
{
	// TODO: VSync.
}

// Set the whether the window is visible.
void Window::SetVisibility(bool isVisible)
{
	ShowWindow(m_handle, isVisible ? SW_SHOW : SW_HIDE);
}

// Set whether the mouse is visible in the window.
void Window::SetMouseVisibility(bool isMouseVisible)
{
	m_mouseVisible = isMouseVisible;
	DoSetMouseVisibility(m_mouseVisible);
}

void Window::SetMousePosition(int x, int y)
{
	SetCursorPos(x, y);
}

// Set whether the mouse should be confined to the window bounds.
void Window::ConfineMouseToWindow(bool confineMouse)
{
	m_mouseConfined = confineMouse;
	DoConfineMouse(m_mouseConfined);
}

void Window::SetFullscreen(bool fullscreen)
{
	if (fullscreen == IsFullscreen())
		return;

	if (fullscreen)
	{
		// Remember the windowed-mode position & size.
		GetWindowRect(m_handle, &m_windowedWindowRect);
		
		// Change the window flags.
		WindowFlags::value_type flags = m_windowSettings.GetFlags();
		flags &= ~WindowFlags::k_fullscreen;
		flags |= WindowFlags::k_fake_fullscreen;
		m_windowSettings.SetFlags(flags);
		
		// Get the appropriate window styles for fullscreen-mode.
		DWORD windowStyle;
		DWORD windowExStyle;
		GetWindowStyles(m_windowSettings.GetFlags(), windowStyle, windowExStyle);
		SetWindowLong(m_handle, GWL_STYLE, (LONG) windowStyle);
		SetWindowLong(m_handle, GWL_EXSTYLE, (LONG) windowExStyle);

		// Update the graphics mode's window size to be full screen.
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		m_graphicsMode.SetWidth(screenWidth);
		m_graphicsMode.SetHeight(screenHeight);
		printf("New size: %d x %d\n", screenWidth, screenHeight);

		// Resize the window so that it fits the entire screen.
		SetWindowPos(m_handle, HWND_TOP, 0, 0, m_graphicsMode.GetWidth(),
			m_graphicsMode.GetHeight(), SWP_FRAMECHANGED);
	}
	else
	{
		// Update the window flags.
		WindowFlags::value_type flags = m_windowSettings.GetFlags();
		flags &= ~WindowFlags::k_fullscreen;
		flags &= ~WindowFlags::k_fake_fullscreen;
		m_windowSettings.SetFlags(flags);

		// Get the appropriate window styles for windowed-mode.
		DWORD windowStyle;
		DWORD windowExStyle;
		GetWindowStyles(m_windowSettings.GetFlags(), windowStyle, windowExStyle);
		SetWindowLong(m_handle, GWL_STYLE, (LONG) windowStyle);
		SetWindowLong(m_handle, GWL_EXSTYLE, (LONG) windowExStyle);

		// Return the window to its previous windowed-mode size and positions.
		SetWindowPos(m_handle, HWND_NOTOPMOST,
			m_windowedWindowRect.left, m_windowedWindowRect.top, 
			m_windowedWindowRect.right - m_windowedWindowRect.left,
			m_windowedWindowRect.bottom - m_windowedWindowRect.top,
			SWP_FRAMECHANGED);

		// Update the graphics mode's window size.
		RECT clientRect;
		GetClientRect(m_handle, &clientRect);
		m_graphicsMode.SetWidth(clientRect.right - clientRect.left);
		m_graphicsMode.SetHeight(clientRect.bottom - clientRect.top);
		printf("New size: %d x %d\n", m_graphicsMode.GetWidth(), m_graphicsMode.GetHeight());
	}
}

// Swap the window's buffers (Push the back buffer to the screen).
void Window::SwapBuffers()
{
	VERIFY_DEVICE_CONTEXT();
	VERIFY_OPENGL_CONTEXT();

	// We have a name clash, make sure we are looking in the global scope
	::SwapBuffers(m_deviceContext);
}


//-----------------------------------------------------------------------------
// Window Events
//-----------------------------------------------------------------------------
	
// Process window events.
void Window::ProcessEvents()
{
	// SFML: Update the window only if we own it
	if (!m_callbackPtr)
	{
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}
	
bool Window::GetEvent(WindowEvent& eventOut)
{
	// If the stack is empty, then see if the window has more events for us
	if (m_events.empty())
	{
		ProcessEvents();
	}
		
	if (!m_events.empty())
	{
		eventOut = m_events.front();
		m_events.pop();
		return true;
	}
		
	return false;
}

void Window::SendEvent(const WindowEvent& windowEvent)
{
	m_events.push(windowEvent);
}


//-----------------------------------------------------------------------------
// Active Window
//-----------------------------------------------------------------------------

Window* Window::GetCurrentActiveWindow()
{
	return g_currentActiveWindow;
}
	
void Window::SetCurrentActiveWindow(Window* window)
{
	g_currentActiveWindow = window;
}

void Window::SetNonActiveWindow(Window* window)
{
	if (window == g_currentActiveWindow)
	{
		g_currentActiveWindow = nullptr;
	}
}

	

//-----------------------------------------------------------------------------
// Private window implementation methods
//-----------------------------------------------------------------------------

// Create and register the window class.
void Window::DoRegisterWindowClass()
{
	// TODO: Register class for non-unicode OS (older windows)

	// Create the window class.
	WNDCLASSW windowClass;
	windowClass.style         = 0;								// Nothing special for the window style
	windowClass.lpfnWndProc   = &Window::DoWindowProcedure;		// Callback for the windows procedure
	windowClass.cbClsExtra    = 0;								// No extra window data
	windowClass.cbWndExtra    = 0;								// No extra window data
	windowClass.hInstance     = GetModuleHandle(nullptr);		// Set the instance
	windowClass.hIcon         = nullptr;						// No icon
	windowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);	// Load the arrow pointer;
	windowClass.hbrBackground = nullptr;						// The application will paint its own background
	windowClass.lpszMenuName  = nullptr;						// We don't want a menu
	windowClass.lpszClassName = g_className;					// Set the class name
		
	// Register the window class.
	ATOM result = RegisterClassW(&windowClass);
	CMG_UNUSED(result);
	CMG_ASSERT_MSG(result, "Failed to register the window class");
}

// Create the render context
void Window::DoCreateContext(const GraphicsMode& graphicsMode, WindowSettings& windowSettings)
{
	// Retrieve the window's device context.
	m_deviceContext = GetDC(m_handle);
	CMG_ASSERT_MSG(m_deviceContext, "Failed to get window's device context");

	int bestPixFormat = 0;

	// TODO: Add anti-alias support here
	/*
	if (windowSettings.GetAntiAlias() > 0)
	{
		// Since we don't have GLEW yet, we will have to manually get the function
		// which will in turn give us the pixel format
		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB =
			reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>
			(wglGetProcAddress("wglChoosePixelFormatARB"));

		if (wglChoosePixelFormatARB)
		{
			int pixAttribs[] =
			{
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_ACCELERATION_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
				WGL_SAMPLE_BUFFERS_ARB, (a_settings.GetAntiAlias() ? GL_TRUE : GL_FALSE),
				WGL_SAMPLES_ARB, (a_settings.GetAntiAlias()),
				0, 0
			};

			// Grab all the formats
			const int maxFormatsToReturn = 256;
			int formats[maxFormatsToReturn];
			UINT numFormats;
			FLOAT attributeList[] = { 0, 0 };

			bool isValid = wglChoosePixelFormatARB(m_deviceContext, pixAttribs,
				attributeList, maxFormatsToReturn, formats, &numFormats) != 0;

			if (isValid == false || numFormats == 0)
			{
				if (a_settings.GetAntiAlias() > 2)
				{
					//TLOC_LOG_GFX_WARN() << "Failed to find a format that supports the "
						//<< "required AA, trying lower levels";

					a_settings.SetAntiAlias(2);
					pixAttribs[11] = a_settings.GetAntiAlias();
					isValid = wglChoosePixelFormatARB(m_deviceContext, pixAttribs,
						attributeList, maxFormatsToReturn, formats, &numFormats) != 0;
				}

				if (isValid == false || numFormats == 0)
				{
					//TLOC_LOG_GFX_WARN() << "Cannot find pixel format supporting any AA"
						//<< ", disabling AA";
					a_settings.SetAntiAlias(0);
				}
			}

			if (isValid == false && numFormats > 0)
			{
				// TODO: Select the best format available
				//CMG_ASSERT_WIP();
			}

		}
		else
		{
			//TLOC_LOG_GFX_WARN() << "AA not supported and will be disabled";
			windowSettings.SetAntiAlias(0);
		}
	}
	*/

	// Find the pixel format closest to what we want.
	if (bestPixFormat == 0)
	{
		PIXELFORMATDESCRIPTOR pixelDesc;
		const size_t pixelDescSize = sizeof(pixelDesc);
		ZeroMemory(&pixelDesc, pixelDescSize);

		// Describe the desired pixel format for the device context.
		pixelDesc.nSize			= pixelDescSize;
		pixelDesc.nVersion		= 1;
		pixelDesc.iLayerType	= PFD_MAIN_PLANE;
		pixelDesc.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pixelDesc.iPixelType	= PFD_TYPE_RGBA;
		pixelDesc.cColorBits	= static_cast<BYTE>(graphicsMode.GetBitsPerPixel());
		pixelDesc.cDepthBits	= static_cast<BYTE>(windowSettings.GetDepthBits());
		pixelDesc.cStencilBits	= static_cast<BYTE>(windowSettings.GetStencilBits());
		pixelDesc.cAlphaBits	= (graphicsMode.GetBitsPerPixel() == 32 ? 8 : 0);
			
		// Get the best available match of pixel format for the device context.
		bestPixFormat = ChoosePixelFormat(m_deviceContext, &pixelDesc);
		CMG_ASSERT_MSG(bestPixFormat, "Failed to find a suitable pixel format for the "
			"device context -- cannot create OpenGL context");
	}

	// Create a descriptor for the pixel format.
	PIXELFORMATDESCRIPTOR pixFormat;
	const size_t pixFormatSize = sizeof(pixFormat);
	pixFormat.nSize = pixFormatSize;
	pixFormat.nVersion = 1;
	DescribePixelFormat(m_deviceContext, bestPixFormat, pixFormatSize, &pixFormat);
	windowSettings.SetDepthBits(pixFormat.cDepthBits);
	windowSettings.SetStencilBits(pixFormat.cStencilBits);

	// Now actually set the context's pixel format.
	bool setPixelFormatResult = SetPixelFormat(m_deviceContext, bestPixFormat, &pixFormat) != 0;
	CMG_UNUSED(setPixelFormatResult);
	CMG_ASSERT_MSG(setPixelFormatResult,
		"SetPixelFormat() failed -- cannot create OpenGL context");

	// Create an OpenGL context for the device.
	m_openGLContext = wglCreateContext(m_deviceContext);
	CMG_ASSERT_MSG(m_openGLContext, "Failed to create OpenGL context");
	
	// Show the window and make the context's current.
	wglMakeCurrent(m_deviceContext, m_openGLContext);
		
	//m_parentWindow->SetActive(true);
	
	// Initialize graphics libraries (GLEW).
	glewInit();

	// Print out graphics platform information.
	std::cout << "-------------- GFX Platform Info --------------" << std::endl;
	std::cout << "OpenGL Version: " << (const char*) glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version: " << (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Vendor: " << (const char*) glGetString(GL_VENDOR) << std::endl;
	std::cout << "Hardware: " << (const char*) glGetString(GL_RENDERER) << std::endl;
	//std::cout << "Total VertexAttribArrays: " << Get<p_get::MaxVertexAttribs>();
	//std::cout << "Total TextureUnits: " << Get<p_get::MaxCombinedTextureImageUnits>();
	std::cout << "-----------------------------------------------" << std::endl;
	
	
	ShowWindow(m_handle, SW_SHOW);
	SetForegroundWindow(m_handle);	// Slightly higher priority.
	SetFocus(m_handle);				// Sets keyboard focus to the window.

	if (windowSettings.GetAntiAlias() > 0)
	{
		//glEnable(GL_MULTISAMPLE_ARB);
	}
}
	
// Switch the window to fullscreen mode (REAL fullscreen mode)
void Window::DoSwitchToFullscreen(const GraphicsMode& graphicsMode)
{
	DEVMODE devMode;
	devMode.dmSize = sizeof(devMode);
	devMode.dmPelsWidth		= (DWORD) graphicsMode.GetWidth(); //core_utils::CastNumber<DWORD>(props.m_width);
	devMode.dmPelsHeight	= (DWORD) graphicsMode.GetHeight(); //core_utils::CastNumber<DWORD>(props.m_height);
	devMode.dmBitsPerPel	= (DWORD) graphicsMode.GetBitsPerPixel(); // core_utils::CastNumber<DWORD>(props.m_bitsPerPixel);
	devMode.dmFields		= DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		
	int result = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
	if (result != DISP_CHANGE_SUCCESSFUL)
	{
		//TLOC_LOG_GFX_WARN() << "Could not change display to full-screen";
		return;
	}

	// SFML: Make the window flags compatible with fullscreen mode
	SetWindowLong(m_handle, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	SetWindowLong(m_handle, GWL_EXSTYLE, WS_EX_APPWINDOW);

	// SFML: Resize the window so that it fits the entire screen
	SetWindowPos(m_handle, HWND_TOP, 0, 0, graphicsMode.GetWidth(),
		graphicsMode.GetHeight(), SWP_FRAMECHANGED);
	ShowWindow(m_handle, SW_SHOW);

	g_currFullScreenWindow = this;
}

void Window::DoSetMouseVisibility(bool isMouseVisible)
{
	// The cursor is displayed only if the display count >= 0.
	// ShowCursor(TRUE) increments the display count by one.
	// ShowCursor(FALSE) decrements the display count by one.
	// ShowCursor() returns the new value of the display counter.
	if (isMouseVisible)
	{
		int displayCount = ShowCursor(TRUE);
		while (displayCount < 0)
		  displayCount = ShowCursor(TRUE);
	}
	else
	{
		int displayCount = ShowCursor(FALSE);
		while (displayCount >= 0)
		  displayCount = ShowCursor(FALSE);
	}
}

void Window::DoConfineMouse(bool confineMouse)
{
	if (confineMouse)
	{
		// Get the screen coordinate for the top-left corner of the client rectangle.
		POINT point = { 0, 0 };
		ClientToScreen(m_handle, &point);
		
		// Get the screen coordinates of the full client rectangle.
		RECT clipRect;
		GetClientRect(m_handle, &clipRect);
		clipRect.left	+= point.x;
		clipRect.top	+= point.y;
		clipRect.right	+= point.x;
		clipRect.bottom	+= point.y;

		// Clip the cursor to the window's client rectangle.
		ClipCursor(&clipRect);

		// FIXME: This will act wierd if the window is moved
		// Ex: If the user grabs the title bar, the window will drop to the bottom of the screen.
	}
	else
	{
		// The cursor is free to move anywhere on the screen.
		ClipCursor(nullptr);
	}
}

// Cleanup the window, releasing its contexts.
void Window::DoCleanup()
{
	// SFML: Restore the previous video mode (in case we are fullscreen)
	if (g_currFullScreenWindow == this)
	{
		ChangeDisplaySettings(nullptr, 0);
		g_currFullScreenWindow = nullptr;
	}

	ConfineMouseToWindow(false);
	SetMouseVisibility(true);

	// Delete the render context.
	if (m_openGLContext)
	{
		if (!IsActive())
			SetActive(true);

		wglDeleteContext(m_openGLContext);
		m_openGLContext = nullptr;

		SetActive(false);
			
	}
		
	// Release the device context.
	if (m_deviceContext)
	{
		ReleaseDC(m_handle, m_deviceContext);
		m_deviceContext = nullptr;
	}
}

// Process a single windows event.
void Window::DoProcessEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_handle == nullptr) { return; }
	
	HWND activeWindowHandle = GetActiveWindow();

	// To ensure the mouse doesn't disappear or remains confined, the following 
	// is necessary to check whether this is the active window because 
	// DoProcessEvent is sometimes called when another window is active. 
	if (activeWindowHandle != m_handle)
	{
		DoSetMouseVisibility(true);
		DoConfineMouse(false);
	}
	else
	{
		DoSetMouseVisibility(m_mouseVisible);
		DoConfineMouse(m_mouseConfined);
	}

	switch (message)
	{
		case WM_DESTROY:
		{
			DoSetMouseVisibility(true);
			DoConfineMouse(false);
			SendEvent(WindowEvent(WindowEvent::k_destroy, GetWidth(), GetHeight()));
			DoCleanup();
			break;
		}
		case WM_CLOSE:
		{
			//SetMouseVisibility(true);
			//DoConfineMouse(false);
			SendEvent(WindowEvent(WindowEvent::k_close, GetWidth(), GetHeight()));
			break;
		}
		case WM_SIZE:
		{
			m_graphicsMode.SetWidth(LOWORD(lParam));
			m_graphicsMode.SetHeight(HIWORD(lParam));
			SendEvent(WindowEvent(WindowEvent::k_resized, GetWidth(), GetHeight()));
			break;
		}
		case WM_SETFOCUS:
		{
			SendEvent(WindowEvent(WindowEvent::k_gained_focus, GetWidth(), GetHeight()));
			break;
		}
		case WM_KILLFOCUS:
		{
			DoSetMouseVisibility(true);
			DoConfineMouse(false);
			SendEvent(WindowEvent(WindowEvent::k_lost_focus, GetWidth(), GetHeight()));
			break;
		}
		case WM_MOUSEMOVE:
		{
			break;
		}
		case WM_DROPFILES:
		{
			HDROP hdrop = (HDROP) wParam;
			SendEvent(WindowEvent(WindowEvent::k_drop_file, GetWidth(), GetHeight()));
			break;
		}
			
	}
}
	

//-----------------------------------------------------------------------------
// Windows Procedure Callback
//-----------------------------------------------------------------------------

// This is the static callback for the window's procedure.
LRESULT CALLBACK Window::DoWindowProcedure(
	HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
{
	// I am not entirely sure why SFML chooses this technique to store the
	// user pointer (that is, wait for the create event and then use a 'trick'
	// to get this class's instance. Should test it later.
	// TODO: Register the user variable in Create()
	if (message == WM_CREATE)
	{
		// Convert LPARAM to a meanigul structure i.e. CREATESTRUCT and then
		// cast the lpCreateParams member to a long which a pointer to
		// WindowImpl<> (this is set by CreateWindowEx function)
		long thisPtr = reinterpret_cast<long>(reinterpret_cast < CREATESTRUCT* >
			(lparam)->lpCreateParams);

		// We set HWND's (handle) user data attribute to thisPtr, which is a
		// Window pointer (this is then queried later by GetWindowLongPtr)
		SetWindowLongPtr(handle, GWLP_USERDATA, thisPtr);
	}

	// Get the Window pointer from the HWND's user data and call 
	// its event processing method.
	Window* win = reinterpret_cast<Window*>(GetWindowLongPtr(handle,
		GWLP_USERDATA));
	if (win)
	{
		win->DoProcessEvent(message, wparam, lparam);
		if (win->m_callbackPtr)
		{
			return CallWindowProc(reinterpret_cast<WNDPROC>(win->m_callbackPtr),
				handle, message, wparam, lparam);
		}
	}

	if (message == WM_CLOSE)
		return 0;

	// Perform the default window procedure.
	return DefWindowProcW(handle, message, wparam, lparam);
}

// Get the appropriate Win32 window style and ex-style given Window Flags
void Window::GetWindowStyles(WindowFlags::value_type windowFlags, DWORD& windowStyle, DWORD& windowExStyle)
{
	// Window Style
	windowStyle = WS_VISIBLE;
	if (windowFlags & WindowFlags::k_fake_fullscreen)
	{
		windowStyle |= WS_POPUP | WS_BORDER;
	}
	else
	{
		if (windowFlags & WindowFlags::k_borderless)
			windowStyle |= WS_POPUP | WS_BORDER;
		else
			windowStyle |= WS_CAPTION | WS_MINIMIZEBOX;
		if (windowFlags & WindowFlags::k_resizable)
			windowStyle |= WS_SIZEBOX | WS_MAXIMIZEBOX;
		if (windowFlags & WindowFlags::k_window_buttons)
			windowStyle |= WS_SYSMENU;
	}

	// Window Ex Style
	windowExStyle = WS_EX_APPWINDOW;
}
