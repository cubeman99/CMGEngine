#ifndef _CMG_WINDOW_H_
#define _CMG_WINDOW_H_

#include <cmgCore/cmg_core.h>
#include <Windows.h>
#include <string>
#include <queue>

//#include <cmgCore/utilities/cmgUtil.h>
//#include <cmgGraphics/window/cmgWindowSettings.h>
//#include <cmgGraphics/window/cmgWindowHandle.h>
//#include <cmgGraphics/cmgGraphicsMode.h>

class Window;


//-----------------------------------------------------------------------------
// GraphicsMode
//-----------------------------------------------------------------------------
struct GraphicsMode
{
public:
	GraphicsMode() {}
		
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	int GetBitsPerPixel() const { return m_bitsPerPixel; }
	int GetRefreshRate() const { return m_refreshRate; }
		
	void SetWidth(int width) { m_width = width; }
	void SetHeight(int height) {  m_height = height; }
	void SetBitsPerPixel(int bitsPerPixel) { m_bitsPerPixel = bitsPerPixel; }
	void SetRefreshRate(int refreshRate) { m_refreshRate = refreshRate; }

private:
	int m_width;
	int m_height;
	int m_bitsPerPixel;
	int m_refreshRate;	// 0 == default (ignored for windowed mode)
};


struct WindowHandle
{
	typedef HWND type;
};


//-----------------------------------------------------------------------------
// WindowFlags
//-----------------------------------------------------------------------------
struct WindowFlags
{
	enum
	{
		k_none				= 0,
		k_resizable			= 0x1,
		k_borderless		= 0x2,
		k_window_buttons	= 0x4,
		k_fullscreen		= 0x8,
		k_fake_fullscreen	= 0x10,
		//k_maximized			= 0x20,
		//k_minimized			= 0x40,
	};

	typedef int value_type;
};


//-----------------------------------------------------------------------------
// WindowSettings
//-----------------------------------------------------------------------------
struct WindowSettings
{
public:
	typedef WindowSettings this_type;
	typedef WindowFlags::value_type flags_type;

public:
	WindowSettings() :
		m_title("Untitled"),
		m_flags(WindowFlags::k_none),
		m_depthBits(16),
		m_stencilBits(0),
		m_antiAlias(0)
	{
	}
		
	inline const std::string& GetTitle() const { return m_title; }
	inline flags_type GetFlags() const { return m_flags; }
	inline unsigned int GetDepthBits() const { return m_depthBits; }
	inline unsigned int GetStencilBits() const { return m_stencilBits; }
	inline int GetAntiAlias() const { return m_antiAlias; }

	inline void SetTitle(const std::string& title) { m_title = title; }
	inline void SetFlags(flags_type flags) { m_flags = flags; }
	inline void SetDepthBits(unsigned int depthBits) { m_depthBits = depthBits; }
	inline void SetStencilBits(unsigned int stencilBits) { m_stencilBits = stencilBits; }
	inline void SetAntiAlias(unsigned int antiAlias) { m_antiAlias = antiAlias; }

private:
	std::string				m_title;
	WindowFlags::value_type	m_flags;
	unsigned int			m_depthBits;
	unsigned int			m_stencilBits;
	int						m_antiAlias;
};



//-----------------------------------------------------------------------------
// Window Event
//-----------------------------------------------------------------------------
struct WindowEvent
{
	enum EventType
	{
		k_none = -1,

		k_close = 0,
		k_resized,
		k_lost_focus,
		k_gained_focus,
		k_destroy,
		k_drop_file,
		k_drop_text,

		k_count,
	};

	WindowEvent() :
		m_type(k_none),
		m_sizeX(0),
		m_sizeY(0)
	{}

	WindowEvent(EventType type) :
		m_type(type),
		m_sizeX(0),
		m_sizeY(0)
	{}

	WindowEvent(const EventType& a_event, size_t a_sizeX, size_t a_sizeY) :
		m_type(a_event),
		m_sizeX(a_sizeX),
		m_sizeY(a_sizeY)
	{}

	inline int GetWidth() const { return m_sizeX; }
	inline int GetHeight() const { return m_sizeY; }

	EventType m_type;
	int m_sizeX;
	int m_sizeY;

	struct
	{
		String text; // Text or filename.
	} drop;
};


//-----------------------------------------------------------------------------
// WindowDropTarget
//-----------------------------------------------------------------------------
class WindowDropTarget : public IDropTarget
{
public:
	WindowDropTarget(Window* window);
	
	// IUnknown interface
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
	ULONG STDMETHODCALLTYPE AddRef() { m_refCount++; return m_refCount; }
	ULONG STDMETHODCALLTYPE Release() { m_refCount--; return m_refCount; }

	// IDropTarget interface
    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) override;
    HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) override;
    HRESULT STDMETHODCALLTYPE DragLeave() override;
    HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) override;

private:
	Window* m_window;
	ULONG m_refCount;
};


//-----------------------------------------------------------------------------
// Window
//-----------------------------------------------------------------------------
class Window
{
public:
	typedef WindowHandle::type      window_handle;
	typedef std::queue<WindowEvent>	event_queue;

public:
	Window();
	~Window();
		
	void Create(const GraphicsMode& graphicsMode, const WindowSettings& windowSettings);
	void Close();
	void SwapBuffers();
	
	void ProcessEvents();
	void SendEvent(const WindowEvent& windowEvent);
	bool GetEvent(WindowEvent& eventOut);

	window_handle GetWindowHandle() const;
	int GetWidth() const;
	int GetHeight() const;
	float GetAspectRatio() const;
	bool IsCreated() const;
	bool IsActive() const;
	bool IsMouseVisible() const;
	bool IsMouseConfined() const;
	bool IsFullscreen() const;
		
	void SetActive(bool isActive);
	void SetSize(int width, int height);
	void SetPosition(int x, int y);
	void SetTitle(const String& title);
	void SetVerticalSync(bool useVSync);
	void SetVisibility(bool isVisible);
	void SetMouseVisibility(bool isMouseVisible);
	void ConfineMouseToWindow(bool confineMouse);
	void SetFullscreen(bool fullscreen);
	
	const GraphicsMode&		GetGraphicsMode()	const { return m_graphicsMode; }
	const WindowSettings&	GetWindowSettings()	const { return m_windowSettings; }

private:
	static LRESULT CALLBACK DoWindowProcedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

	void DoRegisterWindowClass();
	void DoCreateContext(const GraphicsMode& graphicsMode, WindowSettings& windowSettings);
	void DoProcessEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void DoSwitchToFullscreen(const GraphicsMode& graphicsMode);
	void DoSetMouseVisibility(bool isMouseVisible);
	void DoConfineMouse(bool confineMouse);
	void DoCleanup();
	void GetWindowStyles(WindowFlags::value_type windowFlags, DWORD& windowStyle, DWORD& windowExStyle);

	event_queue		m_events;
	WindowSettings	m_windowSettings;
	GraphicsMode	m_graphicsMode;
	bool			m_mouseVisible;
	bool			m_mouseConfined;

	window_handle	m_handle;
	size_t			m_callbackPtr;
	HCURSOR			m_cursor;
	HICON			m_icon;
	bool			m_isCursorIn;
	HDC				m_deviceContext;
	HGLRC			m_openGLContext;
	RECT			m_defaultMouseClip; // Screen coordinates of the rectangular area to which the cursor is confined.

	WindowDropTarget	m_dropTarget;
	
	
	RECT			m_windowedWindowRect;
	

protected:

	static Window* GetCurrentActiveWindow();
		
	// Sets the current active window. This is primarily when a window gains focus
	static void SetCurrentActiveWindow(Window* window);

	// Sets the window as non active. This is primarily when a window loses
	// focus. If it is the current active window, the current active window is
	// set to null.
	static void SetNonActiveWindow(Window* window);
};


#endif // _CMG_WINDOW_H_