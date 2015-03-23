#pragma once

#include "GameObject.h"
#include "Missile.h"
#include "ItemBox.h"
#include "MovableText.h"
#include "OBJ.h"

class Car : public GameObject
{
public:
	Car(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, btDiscreteDynamicsWorld* mWorld, Ogre::String carEntName, int ID, std::shared_ptr<Ogre::Camera> mCameras[4]);
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

	int stunCounter = 0;

	btAlignedObjectArray<btCollisionShape*> mCollisionShapes;
	btRaycastVehicle::btVehicleTuning       m_tuning;
	btVehicleRaycaster*     m_vehicleRayCaster;
	btRaycastVehicle*       m_vehicle;
	btCollisionShape*       m_wheelShape;
	btDiscreteDynamicsWorld* world;

	virtual void InitRigidBody();

	ITEM_BOX_TYPE mCurrentItem;

	bool mFinishedRace = false;

	Ogre::MovableText* positionText;
	Ogre::MovableText* positionModText;
	Ogre::MovableText* lapText;

	unsigned int lastCheckpoint=0;
	unsigned int lastItemBoxCheckpoint = 0;

	void SetItem(ITEM_BOX_TYPE type);

private:
	float speed = 0.0f;
	float turnSpeed = 0.3f;

	int engineForce;
	int brakeForce;
	float steerValue;
	float rollValue;

	Ogre::SceneNode* mCountdownNode;
	Ogre::MovableText* mCountdownText;
	float countdownTime = 0.0f;

	Ogre::SceneNode* mCamera;
	Ogre::Vector3 mCameraMin, mCameraMax;

	Ogre::SceneNode* itemNode;
	Ogre::Entity* itemEnt;
};