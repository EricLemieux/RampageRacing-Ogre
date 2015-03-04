#pragma once 

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Xinput.h>

class Controller
{
public:
	Controller(int playerNumber);
	~Controller();

	XINPUT_STATE GetState();
	bool IsConnected();
	void Vibrate(float leftStrengthPercentage, float rightStrengthPercentage);

private:
	XINPUT_STATE mControllerState;
	int mControllerNum;
};