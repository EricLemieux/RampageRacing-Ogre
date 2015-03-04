#pragma once

#include "GameObject.h"
#include "Missile.h"

class Car : public GameObject
{
public:
	Car(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, btDiscreteDynamicsWorld* mWorld, Ogre::String carEntName);
	~Car();

public:
	void MoveForward(float distance);
	void MoveBackward(float distance);

	void TurnRight(float value=0.3f);
	void TurnLeft(float value=0.3f);

	void FireMissile(Ogre::Entity* MissileEnt);

	virtual void Update(void);

	bool mCanMoveForward	= false;
	bool mCanMoveBackward	= false;
	bool isColliding		= false;

	bool mTurningRight = false;
	bool mTurningLeft  = false;

	int lapCounter;
	int checkPointsHit;

	btAlignedObjectArray<btCollisionShape*> mCollisionShapes;
	btRaycastVehicle::btVehicleTuning       m_tuning;
	btVehicleRaycaster*     m_vehicleRayCaster;
	btRaycastVehicle*       m_vehicle;
	btCollisionShape*       m_wheelShape;
	btDiscreteDynamicsWorld* world;

	virtual void InitRigidBody();

private:
	float speed = 0.0f;
	float turnSpeed = 0.3f;

	int engineForce;
	float steerValue;

	Missile* mMissile;
	bool missileActive = false;
};