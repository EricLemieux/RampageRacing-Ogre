#include "Car.h"

Car::Car() : GameObject()
{
}

Car::Car(Ogre::String name, Ogre::SceneNode* root) : GameObject(name, root)
{
	int a = 0;
}

Car::~Car()
{
}

void Car::MoveForward(float distance)
{
	GetRigidBody()->applyCentralForce(btVector3(0,distance,0));
}

void Car::UpdateSceneNodeFromRigidBody(void)
{
	btTransform trans = GetRigidBody()->getWorldTransform();
	GetSceneNode()->getParent()->setOrientation(BtToOgreQuaternion(&trans.getRotation()));
	GetSceneNode()->getParent()->setPosition(BtToOgreVector3(&trans.getOrigin()));
}