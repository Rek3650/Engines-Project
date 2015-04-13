#include "XboxController.h"

XboxController::XboxController(int playerNumber)
{
	// Set the Controller Number
	_controllerNum = playerNumber;
	
	//first controller update
	// Zeroise the state
	ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

	// Get current state
	XInputGetState(_controllerNum, &_controllerState);
}

void XboxController::UpdateStates()
{
	// Zero out last state
	ZeroMemory(&_lastControllerState, sizeof(XINPUT_STATE));

	// set last state
	_lastControllerState = _controllerState;

	// Zeroise the state
	ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

	// Get current state
	XInputGetState(_controllerNum, &_controllerState);
}

XINPUT_STATE* XboxController::getState()
{
	return &_controllerState;
}

XINPUT_STATE* XboxController::getLastState()
{
	return &_lastControllerState;
}



bool XboxController::IsConnected()
{
	// Zeroise the state
	ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

	// Get the state
	DWORD Result = XInputGetState(_controllerNum, &_controllerState);

	if(Result == ERROR_SUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void XboxController::Vibrate(int leftVal, int rightVal)
{
	// Create a Vibraton State
	XINPUT_VIBRATION Vibration;

	// Zeroise the Vibration
	ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

	// Set the Vibration Values
	Vibration.wLeftMotorSpeed = leftVal;
	Vibration.wRightMotorSpeed = rightVal;

	// Vibrate the controller
	XInputSetState(_controllerNum, &Vibration);
}