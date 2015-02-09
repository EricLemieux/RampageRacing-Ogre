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
	btScalar mat[16];
	GetRigidBody()->getWorldTransform().getOpenGLMatrix(mat);

	btVector3 forward = btVector3(mat[8], mat[9], mat[10]);

	forward *= -distance;

	GetRigidBody()->setLinearVelocity(forward);
}

void Car::MoveBackward(float distance)
{
	MoveForward(-distance/2.0f);
}

void Car::TurnRight(float value)
{
	GetRigidBody()->setAngularVelocity(btVector3(0.0f, -value, 0.0f));
}
void Car::TurnLeft(float value)
{
	TurnRight(-value);
}

void Car::Update(void)
{
	//Move the car if the button is down
	if (mCanMoveForward)
	{
		MoveForward(speed);
	}
	else if (mCanMoveBackward)
	{
		MoveBackward(speed);
	}

	//Update the position of the scene node to match the new position of the rigid body
	btTransform trans = GetRigidBody()->getWorldTransform();
	GetSceneNode()->getParentSceneNode()->setOrientation(BtToOgreQuaternion(&trans.getRotation()));
	GetSceneNode()->getParentSceneNode()->setPosition(BtToOgreVector3(&trans.getOrigin()));
}