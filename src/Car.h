#pragma once

#include "GameObject.h"
#include "Missile.h"
#include "ItemBox.h"

class Car : public GameObject
{
public:
	Car(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, btDiscreteDynamicsWorld* mWorld, Ogre::String carEntName, int ID);
	~Car();

public:
	void MoveForward(float distance);
	void MoveBackward(float distance);

	void TurnRight(float value=0.3f);
	void TurnLeft(float value=0.3f);

	virtual void Update(void);

	//The percentage of the engine force used to move
	float mCanMoveForward	= 0.0f;
	float mCanMoveBackward	= 0.0f;
	bool isColliding		= false;

	float mTurning = 0.0f;

	int lapCounter;
	int checkPointsHit;

	btAlignedObjectArray<btCollisionShape*> mCollisionShapes;
	btRaycastVehicle::btVehicleTuning       m_tuning;
	btVehicleRaycaster*     m_vehicleRayCaster;
	btRaycastVehicle*       m_vehicle;
	btCollisionShape*       m_wheelShape;
	btDiscreteDynamicsWorld* world;

	virtual void InitRigidBody();

	ITEM_BOX_TYPE mCurrentItem;

	bool mFinishedRace = false;

private:
	float speed = 0.0f;
	float turnSpeed = 0.3f;

	int engineForce;
	int brakeForce;
	float steerValue;
	float rollValue;
};