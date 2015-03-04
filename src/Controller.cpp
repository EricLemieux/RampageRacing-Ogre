#include "Controller.h"

Controller::Controller(int playerNumber)
{
	mControllerNum = playerNumber;
}
Controller::~Controller()
{

}

XINPUT_STATE Controller::GetState()
{
	ZeroMemory(&mControllerState, sizeof(XINPUT_STATE));

	XInputGetState(mControllerNum, &mControllerState);

	return mControllerState;
}
bool Controller::IsConnected()
{
	ZeroMemory(&mControllerState, sizeof(XINPUT_STATE));

	DWORD res = XInputGetState(mControllerNum, &mControllerState);

	if (res == ERROR_SUCCESS)
		return true;
	return false;
}
void Controller::Vibrate(float leftStrengthPercentage, float rightStrengthPercentage)
{
	XINPUT_VIBRATION vibration;

	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

	vibration.wLeftMotorSpeed = int(leftStrengthPercentage * 65535);
	vibration.wRightMotorSpeed = int(rightStrengthPercentage * 65535);

	XInputSetState(mControllerNum, &vibration);
}