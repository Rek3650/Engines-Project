#include "InputManager.h"

InputManager::InputManager(HINSTANCE appInst, HWND window, int screenWidth, int screenHeight)
{
	keyboard = new Keyboard();
	keyboard->Initialize(appInst,window,screenWidth,screenHeight);

	xboxC = new XboxController*[2];

	xboxC[0] = new XboxController(0);
	xboxC[1] = new XboxController(1);
	
	xboxCState = new XINPUT_STATE*[2];
	lastXboxCState = new XINPUT_STATE*[2];
}


InputManager::~InputManager(void)
{
	delete keyboard;
	delete xboxC[0];
	delete xboxC[1];
	delete xboxC;
	delete xboxCState;
	delete lastXboxCState;
}

void InputManager::init()
{
	keyboard->Frame();
	keys = keyboard->getKeyboardState();
	lastKeys = keyboard->getLastKeyboardState();

	xboxC[0]->UpdateStates();
	xboxC[1]->UpdateStates();

	xboxCState[0] = xboxC[0]->getState(); 
	xboxCState[1] = xboxC[1]->getState(); 
	lastXboxCState[0] = xboxC[0]->getLastState();
	lastXboxCState[1] = xboxC[1]->getLastState();
}

void InputManager::update()
{
	keyboard->Frame(); //gets the keyboard and mouse state.
	xboxC[0]->UpdateStates();
	xboxC[1]->UpdateStates();	
}

void InputManager::onResize(int width, int height)
{
	keyboard->m_screenWidth = width;
	keyboard->m_screenHeight = height;
}

// handles mouse and keyboard input
#pragma region mouse/keyboard

bool InputManager::getKey(int keycode)
{
	if(keys[keycode])
		return true;
	return false;
}


bool InputManager::onKeyDown(int keycode)
{
	if(keys[keycode] && !(lastKeys[keycode]))
		return true;
	return false;
}

bool InputManager::onKeyUp(int keycode)
{
	if(!(keys[keycode]) && lastKeys[keycode])
		return true;
	return false;
}

bool InputManager::getMouse(int mouseButton)
{
	BYTE* mouseState = keyboard->getMouseState();
	switch(mouseButton)
	{
		case 0:
			if(mouseState[0]) return true;
			break;
		case 1:
			if(mouseState[1]) return true;
			break;
		case 2:
			if(mouseState[2]) return true;
			break;
		default:
			return false;
			break;
	}
	return false;
}

bool InputManager::onMouseDown(int mouseButton)
{
	BYTE* mouseState = keyboard->getMouseState();
	BYTE* lastMouseState = keyboard->getLastMouseState();
	switch(mouseButton)
	{
		case 0:
			if(mouseState[0] && !(lastMouseState[0])) return true;
			break;
		case 1:
			if(mouseState[1] && !(lastMouseState[1])) return true;
			break;
		case 2:
			if(mouseState[2] && !(lastMouseState[2])) return true;
			break;
		default:
			return false;
			break;
	}
	return false;
}

bool InputManager::onMouseUp(int mouseButton)
{
	BYTE* mouseState = keyboard->getMouseState();
	BYTE* lastMouseState = keyboard->getLastMouseState();
	switch(mouseButton)
	{
		case 0:
			if(!(mouseState[0]) && lastMouseState[0]) return true;
			break;
		case 1:
			if(!(mouseState[1]) && lastMouseState[1]) return true;
			break;
		case 2:
			if(!(mouseState[2]) && lastMouseState[2]) return true;
			break;
		default:
			return false;
			break;
	}
	return false;
}

XMFLOAT2 InputManager::getMousePos()
{
	int x, y = 0;
	keyboard->GetMouseLocation(x, y);
	return XMFLOAT2(x, y);
}

int InputManager::getMouseWheel()
{
	return keyboard->getMouseWheel();
}

#pragma endregion

// handles controller input
#pragma region XboxController

bool InputManager::isControllerConnected(int cNum)
{
	return xboxC[cNum]->IsConnected();
}

//current
float InputManager::getRightStickX(int cNum)
{
	return xboxCState[cNum]->Gamepad.sThumbRX/32768.0f;
}

float InputManager::getRightStickY(int cNum)
{
	return xboxCState[cNum]->Gamepad.sThumbRY/32768.0f;
}

float InputManager::getLeftStickX(int cNum)
{
	return xboxCState[cNum]->Gamepad.sThumbLX/32768.0f;
}

float InputManager::getLeftStickY(int cNum)
{
	return xboxCState[cNum]->Gamepad.sThumbLY/32768.0f;
}

float InputManager::getRightTrigger(int cNum)
{
	return xboxCState[cNum]->Gamepad.bRightTrigger/255.0f;
}

float InputManager::getLeftTrigger(int cNum)
{
	return xboxCState[cNum]->Gamepad.bLeftTrigger/255.0f;
}

// previous
float InputManager::getPrevRightStickX(int cNum)
{
	return lastXboxCState[cNum]->Gamepad.sThumbRX/32768.0f;
}

float InputManager::getPrevRightStickY(int cNum)
{
	return lastXboxCState[cNum]->Gamepad.sThumbRY/32768.0f;
}

float InputManager::getPrevLeftStickX(int cNum)
{
	return lastXboxCState[cNum]->Gamepad.sThumbLX/32768.0f;
}

float InputManager::getPrevLeftStickY(int cNum)
{
	return lastXboxCState[cNum]->Gamepad.sThumbLY/32768.0f;
}

float InputManager::getPrevRightTrigger(int cNum)
{
	return lastXboxCState[cNum]->Gamepad.bRightTrigger/255.0f;
}

float InputManager::getPrevLeftTrigger(int cNum)
{
	return lastXboxCState[cNum]->Gamepad.bLeftTrigger/255.0f;
}


bool InputManager::isXboxButtonPressed(int cNum, int flag)
{
	if(bitChkTwoState(xboxCState[cNum]->Gamepad.wButtons,lastXboxCState[cNum]->Gamepad.wButtons,flag))
		return true;
	else return false;
}

bool InputManager::isXboxButtonDown(int cNum, int flag)
{
	if(bitChk(xboxCState[cNum]->Gamepad.wButtons,flag))
		return true;
	else return false;
}

#pragma endregion