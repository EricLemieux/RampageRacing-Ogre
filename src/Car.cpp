#include "Car.h"

Car::Car() : GameObject()
{
}

Car::Car(Ogre::String name, Ogre::SceneNode* root) : GameObject(name, root)
{
}

Car::~Car()
{
}

void Car::MoveForward(float distance)
{
	GetSceneNode()->translate(distance,0,0);
}