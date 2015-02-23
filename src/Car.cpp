#include "Car.h"

Car::Car(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager) : GameObject(name, manager)
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

	GetRigidBody()->activate();
	GetRigidBody()->setLinearVelocity(forward);
}

void Car::MoveBackward(float distance)
{
	MoveForward(-distance/2.0f);
}

void Car::TurnRight(float value)
{
	GetRigidBody()->activate();
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
		if (speed < 30)
			++speed;
		MoveForward(speed);
	}
	else if (mCanMoveBackward)
	{
		if (speed > -20)
			--speed;
		MoveForward(speed);
	}
	else {
		speed = speed * 0.99f;
		MoveForward(speed);
	}
	

	GameObject::Update();
}