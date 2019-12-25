#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <cmgCore/cmg_core.h>
#include <cmgGraphics/cmg_graphics.h>
#include <cmgInput/cmg_input.h>
#include <cmgApplication/cmgResourceManager.h>


class Application
{
public:
	Application();
	virtual ~Application();

	bool Initialize(const std::string& title, uint32 width, uint32 height);
	void Run();
	void Quit();

	virtual void OnInitialize() {}
	virtual void OnUpdate(float timeDelta) {}
	virtual void OnRender() {}
	virtual void OnQuit() {}
	virtual void OnResizeWindow(int width, int height) {}
	virtual void OnDropFiles(const Array<Path>& paths) {}
	CMG_DEPRECATED virtual void OnDropFile(const String& fileName) {}
	virtual void OnDropText(const String& text) {}
	
	inline float GetFPS() { return m_perceivedFps; }
	inline Window* GetWindow() { return &m_window; }
	inline RenderDevice* GetRenderDevice() { return m_renderDevice; }
	inline InputManager* GetInputManager() { return &m_inputManager; }
	inline cmg::ResourceManager* GetResourceManager() { return &m_resourceManager; }
	inline Mouse* GetMouse() { return m_mouse; }
	inline Keyboard* GetKeyboard() { return m_keyboard; }
	inline cmg::EventManager* GetEventManager() { return &m_eventManager; }


private:
	Window m_window;
	cmg::ResourceManager m_resourceManager;
	cmg::EventManager m_eventManager;
	RenderDevice* m_renderDevice = nullptr;
	InputManager m_inputManager;
	Keyboard* m_keyboard = nullptr;
	Mouse* m_mouse = nullptr;

	float m_perceivedFps = 0.0f;
	bool m_isQuitRequested = false;
	bool m_isInitialized = false;
};


#endif // _APPLICATION_H_