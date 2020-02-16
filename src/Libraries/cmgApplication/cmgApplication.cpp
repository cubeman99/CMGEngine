#include "cmgApplication.h"
#include <GL/GL.h>


Application::Application()
{
}

Application::~Application()
{
	delete m_renderDevice;
	m_renderDevice = nullptr;

	Font::QuitFreeTypeLibrary();
}

bool Application::Initialize(const std::string& title, uint32 width, uint32 height)
{
	GraphicsMode gfxMode;
	gfxMode.SetWidth((int) width);
	gfxMode.SetHeight((int) height);
	gfxMode.SetBitsPerPixel(32);
	gfxMode.SetRefreshRate(60);

	WindowSettings winSettings;
	winSettings.SetTitle(title);
	winSettings.SetDepthBits(16);
	winSettings.SetStencilBits(0);
	winSettings.SetFlags(WindowFlags::k_resizable | WindowFlags::k_window_buttons);

	m_window.Create(gfxMode, winSettings);

	if (!m_inputManager.Initialize(&m_window))
		return false;

	m_renderDevice = new OpenGLRenderDevice(&m_window);
	m_mouse = m_inputManager.AddDevice<Mouse>();
	m_keyboard = m_inputManager.AddDevice<Keyboard>();

	OnInitialize();

	m_isInitialized = true;
	return true;
}

void Application::Quit()
{
	m_isQuitRequested = true;
}

void Application::Run()
{
	if (!m_isInitialized)
		return;

	WindowEvent winEvent;
	m_isQuitRequested = false;

	while (m_window.IsCreated() && !m_isQuitRequested)
	{
		//startTime = clock();

		m_inputManager.UpdateAll();

		// Process window events
		while (m_window.GetEvent(winEvent))
		{
			if (winEvent.m_type == WindowEvent::k_resized)
			{
				Window::ResizedEvent resizedEvent;
				resizedEvent.width = winEvent.GetWidth();
				resizedEvent.height = winEvent.GetHeight();
				m_eventManager.Publish(&resizedEvent);
				OnResizeWindow(winEvent.GetWidth(), winEvent.GetHeight());
				glViewport(0, 0, winEvent.GetWidth(), winEvent.GetHeight());
			}
			else if (winEvent.m_type == WindowEvent::k_close)
			{
				Window::CloseEvent closeEvent;
				m_eventManager.Publish(&closeEvent);
				Quit();
				break;
			}
			else if (winEvent.m_type == WindowEvent::k_drop_file)
			{
				Window::DropFilesEvent dropEvent;
				dropEvent.paths = winEvent.drop.paths;
				m_eventManager.Publish(&dropEvent);
				OnDropFiles(winEvent.drop.paths);
			}
			else if (winEvent.m_type == WindowEvent::k_drop_text)
			{
				Window::DropTextEvent dropEvent;
				dropEvent.text = winEvent.drop.text;
				m_eventManager.Publish(&dropEvent);
				OnDropText(winEvent.drop.text);
			}
			else if (winEvent.m_type == WindowEvent::k_key_typed)
			{
				Window::KeyTypedEvent keyTypedEvent;
				keyTypedEvent.key = winEvent.GetKey();
				keyTypedEvent.keyChar = winEvent.GetKeyChar();
				keyTypedEvent.keyCharUTF32 = winEvent.GetKeyCharUTF32();
				m_eventManager.Publish(&keyTypedEvent);
			}
			else if (winEvent.m_type == WindowEvent::k_key_down)
			{
				Window::KeyDownEvent keyDownEvent;
				keyDownEvent.key = winEvent.GetKey();
				m_keyboard->InjectKeyDownEvent(keyDownEvent.key);
				m_eventManager.Publish(&keyDownEvent);
			}
			else if (winEvent.m_type == WindowEvent::k_key_up)
			{
				Window::KeyUpEvent keyUpEvent;
				keyUpEvent.key = winEvent.GetKey();
				m_keyboard->InjectKeyUpEvent(keyUpEvent.key);
				m_eventManager.Publish(&keyUpEvent);
			}
		}

		// Process input events
		for (auto button : {MouseButtons::left, MouseButtons::right, MouseButtons::middle})
		{
			if (m_mouse->IsButtonPressed(button))
			{
				Window::MouseDownEvent mouseDownEvent;
				mouseDownEvent.button = button;
				mouseDownEvent.location = m_mouse->GetMouseState().location.xy;
				m_eventManager.Publish(&mouseDownEvent);
			}
			else if (m_mouse->IsButtonReleased(button))
			{
				Window::MouseUpEvent mouseUpEvent;
				mouseUpEvent.button = button;
				mouseUpEvent.location = m_mouse->GetMouseState().location.xy;
				m_eventManager.Publish(&mouseUpEvent);
			}
		}

		if (m_isQuitRequested)
			break;

		//m_inputManager.ResetAll();
		//m_inputManager.UpdateAll();

		float timeDelta = 0.016f;
		OnUpdate(timeDelta);

		if (m_isQuitRequested)
			break;
		
		OnRender();
		m_window.SwapBuffers();

		Sleep(10);
		//Sleep(16);
	}
	
	OnQuit();
	m_window.Close();
}

