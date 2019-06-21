#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <cmgCore/cmg_core.h>
#include <cmgGraphics/cmg_graphics.h>
#include <cmgInput/cmg_input.h>


class Application
{
public:
	Application();
	virtual ~Application();

	bool Initialize(const std::string& title, unsigned int width, unsigned int height);
	void Run();
	void Quit();

	virtual void OnInitialize() {}
	virtual void OnUpdate(float timeDelta) {}
	virtual void OnRender() {}
	virtual void OnQuit() {}

	virtual void OnResizeWindow(int width, int height) {}
	virtual void OnDropFile(const String& fileName) {}
	virtual void OnDropText(const String& text) {}


	inline float GetFPS() { return m_perceivedFps; }
	inline Window* GetWindow() { return &m_window; }
	inline InputManager* GetInputManager() { return &m_inputManager; }
	inline Mouse* GetMouse() { return m_mouse; }
	inline Keyboard* GetKeyboard() { return m_keyboard; }


private:
	Window m_window;
	InputManager m_inputManager;
	Keyboard* m_keyboard;
	Mouse* m_mouse;
	float m_perceivedFps;

	bool m_isQuitRequested;
	bool m_isInitialized;
};


#endif // _APPLICATION_H_