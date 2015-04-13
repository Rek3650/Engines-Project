#ifndef _XBOX_CONTROLLER_H_
#define _XBOX_CONTROLLER_H_

// No MFC
#define WIN32_LEAN_AND_MEAN

// We need the Windows Header and the XInput Header
#include <windows.h>
#include <XInput.h>

// Now, the XInput Library
// NOTE: COMMENT THIS OUT IF YOU ARE NOT USING A COMPILER THAT SUPPORTS THIS METHOD OF LINKING LIBRARIES
#pragma comment(lib, "XINPUT9_1_0.lib")

// XBOX Controller Class Definition
class XboxController
{
private:
	XINPUT_STATE _controllerState;
	XINPUT_STATE _lastControllerState;
	int _controllerNum;

public:
	XboxController(int playerNumber);
	void UpdateStates();
	XINPUT_STATE* getState();
	XINPUT_STATE* getLastState();
	bool IsConnected();
	void Vibrate(int leftVal = 0, int rightVal = 0);
};

#endif