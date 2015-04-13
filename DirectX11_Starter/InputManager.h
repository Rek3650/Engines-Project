/*
The keys associated with this class are from Dinput.h

Reference for xbox controllers
http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinput_gamepad(v=vs.85).aspx
*/

#pragma once

#include <dinput.h>
#include "Keyboard.h"
#include "XboxController.h"
#include <DirectXMath.h>

//Bit check & two state.
//Hehe... bitCh... 
#define bitChkTwoState(state, lastState, flag) ((state & flag) == flag) && !((lastState & flag) == flag)
#define bitChk(state, flag) ((state & flag) == flag)

using namespace DirectX;

class InputManager
{
public:
	InputManager(HINSTANCE, HWND, int, int);
	~InputManager(void);
	void update();
	void init();
	bool getKey(int keyCode); // is key pressed.

	
	bool onKeyDown(int keyCode); // was key pressed THIS FRAME.
	bool onKeyUp(int keyCode); // was key released THIS FRAME.

	bool getMouse(int mouseButton); // is mouse button down
	bool onMouseDown(int mouseButton); // was mouse button pressed this frame
	bool onMouseUp(int mouseButton); // was mouse button released this frame
	XMFLOAT2 getMousePos();
	int getMouseWheel();

	//duh
	bool isControllerConnected(int controllerNum);

	//Returns a value from -1 to 1 representing how much the stick is pressed.
	float getRightStickX(int controllerNum);
	float getLeftStickX(int controllerNum);
	float getRightStickY(int controllerNum);
	float getLeftStickY(int controllerNum);

	float getPrevRightStickX(int controllerNum);
	float getPrevLeftStickX(int controllerNum);
	float getPrevRightStickY(int controllerNum);
	float getPrevLeftStickY(int controllerNum);
	
	//returns value between 0 and 1 representing hoe much the trigger is pressed.
	float getRightTrigger(int controllerNum);
	float getLeftTrigger(int controllerNum);

	float getPrevRightTrigger(int controllerNum);
	float getPrevLeftTrigger(int controllerNum);

	//Button presses on XBOX controller, only true on frame the button is pressed.
	//The INT flag passed in is a XINPUT_GAMEPAD define
	bool isXboxButtonPressed(int controllerNum, int flag);
	bool isXboxButtonDown(int controllerNum, int flag);

	void onResize(int width, int height);

private:
	//pointers to the keyboard and controller objects.
	bool* lastKeys;
	bool* keys;
	BYTE* mouse;
	BYTE* lastMouse;
	Keyboard* keyboard;
	XboxController** xboxC;
	XINPUT_STATE** xboxCState;
	XINPUT_STATE** lastXboxCState;
};

