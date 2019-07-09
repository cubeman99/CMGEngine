#include "cmgApplication.h"
#include <GL/GL.h>


Application::Application():
	m_isInitialized(false),
	m_isQuitRequested(false),
	m_perceivedFps(0.0f),
	m_renderDevice(nullptr)
{
}

Application::~Application()
{
	delete m_renderDevice;
	m_renderDevice = nullptr;
}

bool Application::Initialize(const std::string& title, unsigned int width, unsigned int height)
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

		// Process window events.
		while (m_window.GetEvent(winEvent))
		{
			if (winEvent.m_type == WindowEvent::k_resized)
			{
				OnResizeWindow(winEvent.GetWidth(), winEvent.GetHeight());
				glViewport(0, 0, winEvent.GetWidth(), winEvent.GetHeight());
			}
			else if (winEvent.m_type == WindowEvent::k_close)
			{
				Quit();
				break;
			}
			else if (winEvent.m_type == WindowEvent::k_drop_file)
			{
				OnDropFile(winEvent.drop.text);
			}
			else if (winEvent.m_type == WindowEvent::k_drop_text)
			{
				OnDropText(winEvent.drop.text);
			}
		}

		if (m_isQuitRequested)
			break;

		//m_inputManager.ResetAll();
		m_inputManager.UpdateAll();

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

