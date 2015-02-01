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

private:
};