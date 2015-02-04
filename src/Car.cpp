#include "Car.h"

Car::Car() : GameObject()
{
	move = false;
}

Car::Car(Ogre::String name, Ogre::SceneNode* root) : GameObject(name, root)
{
	move = false;
}

Car::~Car()
{
}

void Car::MoveForward(float distance)
{
	GetRigidBody()->applyCentralForce(btVector3(0,distance,0));
}

void Car::Update(void)
{
	//Move the car if the button is down
	if(move)
	{
		MoveForward(99);
	}

	//Update the position of the scene node to match the new position of the rigid body
	btTransform trans = GetRigidBody()->getWorldTransform();
	GetSceneNode()->getParent()->setOrientation(BtToOgreQuaternion(&trans.getRotation()));
	GetSceneNode()->getParent()->setPosition(BtToOgreVector3(&trans.getOrigin()));
}