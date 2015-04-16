#pragma once
//#define DIRECTINPUT_VERSION 0x0800 //might be necessary?

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>
#include <algorithm> 

class Keyboard
{
public:
	Keyboard(void);
	~Keyboard(void);

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);

	bool* getKeyboardState(){ return m_keyboardState;}
	bool* getLastKeyboardState(){ return m_lastKeyboardState;}
	BYTE* getMouseState();
	BYTE* getLastMouseState();
	int getMouseWheel();

	int m_screenWidth, m_screenHeight;

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

	//members
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;
	HWND window;

	//keyboard state
	bool m_keyboardState[256];
	bool m_lastKeyboardState[256];
	DIMOUSESTATE m_mouseState;
	DIMOUSESTATE m_lastMouseState;

	//mouse state
	int m_mouseX, m_mouseY;
};

