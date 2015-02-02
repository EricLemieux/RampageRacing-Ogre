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
	GetSceneNode()->getParentSceneNode()->translate(distance,0,0);
}