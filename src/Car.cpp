#include "Car.h"

Car::Car(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, btDiscreteDynamicsWorld* mWorld, Ogre::String carEntName, int ID, std::shared_ptr<Ogre::Camera> mCameras[4])
{
	mName = name;
	mSceneManager = manager;
	
	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(name);
	Ogre::Entity* someEnt = mSceneManager->createEntity(carEntName);
	someEnt->setCastShadows(true);
	mSceneNode->attachObject(someEnt);

	Ogre::Matrix4 t = mSceneManager->getSceneNode("triggerFinishLine")->_getFullTransform();
	Ogre::Vector3 forward = Ogre::Vector3(t[2][0], t[2][1], t[2][2]);
	
	Ogre::Vector3 pos = Ogre::Vector3(t[0][3], t[1][3], t[2][3]);
	pos += forward.normalisedCopy() * ((ID+1) * -40.0f);
	pos.y += 5;
	mSceneNode->translate(pos);

	mSceneNode->setOrientation(mSceneManager->getSceneNode("triggerFinishLine")->getOrientation());

	world = mWorld;

	lapCounter = 0;
	checkPointsHit = 0;
	engineForce = 0;
	brakeForce = 100;

	objectType = CAR;

	steerValue = 0.0f;  
	rollValue = 0.0f;

	InitRigidBody();

	mCurrentItem = IBT_NONE;

	//Set up the camera
	Ogre::SceneNode* camNode = mSceneManager->getSceneNode(name)->createChildSceneNode();
	camNode->attachObject(mCameras[ID].get());

	mCameraMin = Ogre::Vector3(0.0f, 10.0f, 40.0f);
	mCameraMax = Ogre::Vector3(0.0f, 9.5f, 42.0f);
	camNode->translate(mCameraMin);

	mCameras[ID]->lookAt(this->GetSceneNode()->getPosition());
	this->mCamera = camNode;

	//Set up the HUD elements attached to the camera
	Ogre::SceneNode* positionNumNode = camNode->createChildSceneNode();
	positionNumNode->translate(-5.5, -6, -10);
	char pp[128];
	sprintf_s(pp, 128, "playerPostionNum%i", ID);
	positionText = new Ogre::MovableText(pp, "1");
	positionText->setVisibilityFlags(RenderOnly[ID]);
	positionNumNode->scale(Ogre::Vector3(2, 2, 1));
	positionNumNode->attachObject(positionText);

	Ogre::SceneNode* positionModNode = positionNumNode->createChildSceneNode();
	positionModNode->translate(-0.1, -0.2, 0);
	char ppm[128];
	sprintf_s(ppm, 128, "playerPostionMod%i", ID);
	positionModText = new Ogre::MovableText(ppm, "st");
	positionModText->setVisibilityFlags(RenderOnly[ID]);
	positionModNode->scale(0.3, 0.3, 1);
	positionModNode->attachObject(positionModText);

	Ogre::SceneNode* lapNode = camNode->createChildSceneNode();
	lapNode->translate(5.5, -6, -10);
	char lc[128];
	sprintf_s(lc, 128, "lapCounter%i", ID);
	lapText = new Ogre::MovableText(lc, "Lap 1/3");
	lapText->setVisibilityFlags(RenderOnly[ID]);
	lapNode->attachObject(lapText);

	//Set up the countdown for the start of the race
	mCountdownNode = mSceneNode->createChildSceneNode();
	mCountdownNode->scale(10, 10, 1);
	mCountdownText = new Ogre::MovableText("countdown", "3");
	mCountdownText->setVisibilityFlags(RenderOnly[ID]);
	mCountdownNode->attachObject(mCountdownText);
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
	if (countdownTime < 4.0f)
	{
		countdownTime += 1.0f / 120.0f;

		float rem = float((int)countdownTime) - countdownTime;

		char str[8];
		sprintf_s(str,8,"%i",3-(int)countdownTime);
		mCountdownText->setCaption(str);

		mCountdownNode->setPosition(Ogre::Math::lerp(mCamera->getPosition() + Ogre::Vector3(2, 0, 10), mCamera->getPosition() + Ogre::Vector3(1, 5, 50), rem));
	}
	else	//Only let the car move when the countdown is done
	{
		mCountdownNode->setPosition(0, -300, 0);

		for (int i = 0; i < m_vehicle->getNumWheels(); ++i){
			m_vehicle->updateWheelTransform(i, true);
		}

		float maxSpeed = 100;
		float maxSpeedb = -50;
		float currentSpeed = mRigidBody->getLinearVelocity().norm();
		float speedRatio = 0.01f;
		float speedRatiob = 0.01f;
		if (currentSpeed != 0){
			speedRatio = currentSpeed / maxSpeed;
			speedRatiob = currentSpeed / maxSpeedb;

			//Move the camera based on the speed ratio
			//mCamera->translate(Ogre::Vector3(0,0,speedRatio * 0.01f));
			mCamera->setPosition(Ogre::Math::lerp(mCameraMin, mCameraMax, speedRatio));
		}

		if (stunCounter > 0){
			rollValue *= 0.97;
			steerValue = 0;
			engineForce = 0;
			brakeForce = 100;
			--stunCounter;
		}
		else {
			//Move the car if the button is down
			if (mCanMoveForward != 0.0f && !mFinishedRace)
			{
				if (mCanMoveForward > 1.0f)
					mCanMoveForward = 1.0f;
				engineForce = (-5000.0f / speedRatio) * mCanMoveForward;
				brakeForce = 0;
			}
			else if (mCanMoveBackward != 0.0f && !mFinishedRace)
			{
				if (mCanMoveBackward > 1.0f)
					mCanMoveBackward = 1.0f;
				engineForce = (-5000.f / speedRatiob) * mCanMoveBackward;
				brakeForce = 50;
			}
			else
			{
				engineForce = 0;
				brakeForce = 50;
			}

			if (mTurning != 0.0f && !mFinishedRace)
			{
				rollValue += (0.002f * mTurning);
				steerValue += (0.002f * mTurning);

				TurnRight(steerValue * 10);

				if (rollValue > 0.2f){
					rollValue = 0.2f;
				}
				else if (rollValue < -0.2f){
					rollValue = -0.2f;
				}
				if (steerValue > 0.2f)
					steerValue = 0.2f;
				else if (steerValue < -0.2f)
					steerValue = -0.2f;
			}
			else
			{
				rollValue *= 0.97;
				steerValue = 0;
			}
		}

		int wheelIndex = 0;
		m_vehicle->setBrake(brakeForce, wheelIndex);
		wheelIndex = 1;
		m_vehicle->setBrake(brakeForce, wheelIndex);

		btScalar mat[16];
		mRigidBody->getWorldTransform().getOpenGLMatrix(mat);
		btVector3 forward = btVector3(mat[8], mat[9], mat[10]);
		mRigidBody->applyCentralForce(forward * engineForce);

		wheelIndex = 3;
		wheelIndex = 2;

		ghost->setWorldTransform(mRigidBody->getWorldTransform());
	}

	GameObject::Update();

	mSceneNode->roll(Ogre::Radian(-rollValue*2));
}

void Car::InitRigidBody()
{
	//if mass is zero it counts as infinite
	float mass = 50.0f;

	btQuaternion q = OgreToBtQuaternion(mSceneNode->getOrientation());
	btVector3 v = OgreToBtVector3(mSceneNode->getPosition());

	btTransform transform = btTransform(q, v);

	ghost = new btPairCachingGhostObject();
	ghost->setWorldTransform(transform);
	

	btCollisionShape* boxShape = new btBoxShape(btVector3(10, 5, 15));
	btCompoundShape* compound = new btCompoundShape();
	mCollisionShapes.push_back(boxShape);
	mCollisionShapes.push_back(compound);

	ghost->setCollisionShape(boxShape);
	ghost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

	btTransform t = btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 3, 0));
	compound->addChildShape(t, boxShape);

	btDefaultMotionState* boxMotionState = new btDefaultMotionState(transform);
	btVector3 boxInertia(0, 0, 0);
	boxShape->calculateLocalInertia(mass, boxInertia);
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass, boxMotionState, compound, boxInertia);
	
	mRigidBody = new btRigidBody(boxRigidBodyCI);
	mRigidBody->setActivationState(DISABLE_DEACTIVATION);
	world->addRigidBody(mRigidBody);
	world->addCollisionObject(ghost);
	

	
	float connectionHeight = 1.f;
	float wheelWidth = 1.f;
	float wheelRadius = 3.f;
	int rightIndex = 0;
	int upIndex = 1;
	int forwardIndex = 2;
	btVector3 wheelDirection(0, -1, 0);
	btVector3 wheelAxle(-1, 0, 0);
	float suspensionRestLength = 3.f;
	bool isFrontWheel = true;

	btCollisionShape* wheelShape = new btCylinderShapeX(btVector3(wheelWidth, wheelRadius, wheelRadius));

	m_vehicleRayCaster = new btDefaultVehicleRaycaster(world);
	m_vehicle = new btRaycastVehicle(m_tuning, mRigidBody, m_vehicleRayCaster);

	world->addVehicle(m_vehicle);

	m_vehicle->setCoordinateSystem(rightIndex, upIndex, forwardIndex);
	btVector3 connectionPoint((10.f - wheelWidth), connectionHeight, (10.f - wheelRadius));
	m_vehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
	connectionPoint = btVector3((-10.f - wheelWidth), connectionHeight, (10.f - wheelRadius));
	m_vehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
	connectionPoint = btVector3((-10.f - wheelWidth), connectionHeight, (-10.f - wheelRadius));
	m_vehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, false);
	connectionPoint = btVector3((10.f - wheelWidth), connectionHeight, (-10.f - wheelRadius));
	m_vehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, false);

	float   wheelFriction = 100;
	float   suspensionStiffness = 20.f;
	float   suspensionDamping = 4.3f;
	float   suspensionCompression = 10.4f;
	float   rollInfluence = 0.f;


	for (int i = 0; i < m_vehicle->getNumWheels(); ++i){
		btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = suspensionStiffness;
		wheel.m_wheelsDampingRelaxation = suspensionDamping;
		wheel.m_wheelsDampingCompression = suspensionCompression;
		wheel.m_frictionSlip = wheelFriction;
		wheel.m_rollInfluence = rollInfluence;
	}
	
}