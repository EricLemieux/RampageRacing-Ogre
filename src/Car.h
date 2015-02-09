#pragma once

#include "GameObject.h"

class Car : public GameObject
{
public:
	Car();
	Car(Ogre::String name, Ogre::SceneNode* root);
	~Car();

public:
	void MoveForward(float distance);
	void MoveBackward(float distance);

	void TurnRight(float value=0.3f);
	void TurnLeft(float value=0.3f);

	virtual void Update(void);

	bool mCanMoveForward	= false;
	bool mCanMoveBackward	= false;

private:
	float speed = 99.0f;
	float turnSpeed = 0.3f;
};