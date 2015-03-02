#include "Car.h"

Car::Car(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, btDiscreteDynamicsWorld* mWorld)
{
	mName = name;
	mSceneManager = manager;
	mSceneNode = mSceneManager->getSceneNode(name);
	world = mWorld;

	lapCounter = 0;
	checkPointsHit = 0;

	InitRigidBody();
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

void Car::FireMissile(Ogre::Entity* MissileEnt)
{
	if (missileActive)
	{
		mSceneManager->getRootSceneNode()->removeAndDestroyChild("missile");
	}

	mMissile = new Missile("missile", mSceneManager, MissileEnt, GetSceneNode());

	world->addRigidBody(mMissile->GetRigidBody());

	btScalar mat[16];
	GetRigidBody()->getWorldTransform().getOpenGLMatrix(mat);
	btVector3 forward = btVector3(mat[8], mat[9], mat[10]);
	forward *= -10;
	
	mMissile->setVelocity(forward.x(), forward.y(), forward.z());
	
	missileActive = true;
}

void Car::Update(void)
{
	world->debugDrawWorld();
	for (int i = 0; i < m_vehicle->getNumWheels(); ++i){
		m_vehicle->updateWheelTransform(i, true);
	}
	engineForce = 0;
	steerValue = 0;
	//Move the car if the button is down
	if (mCanMoveForward)
	{
		if (mRigidBody->getLinearVelocity().norm() < 100)
			engineForce = -1000;
	}
	else if (mCanMoveBackward)
	{
		if (mRigidBody->getLinearVelocity().norm() > -50)
			engineForce = 300;
	}
	else {
		engineForce = 0;
	}

	if (mTurningRight)
	{
		steerValue += 0.1f;
		if (steerValue > 0.8f)
			steerValue = 0.8f;
	}
	else if (mTurningLeft)
	{
		steerValue -= 0.1f;
		if (steerValue < -0.8f)
			steerValue = -0.8f;
	}
	
	int wheelIndex = 2;
	m_vehicle->applyEngineForce(engineForce, wheelIndex);
	m_vehicle->setBrake(100, wheelIndex);
	wheelIndex = 3;
	m_vehicle->applyEngineForce(engineForce, wheelIndex);
	m_vehicle->setBrake(100, wheelIndex);


	wheelIndex = 0;
	m_vehicle->setSteeringValue(steerValue, wheelIndex);
	wheelIndex = 1;
	m_vehicle->setSteeringValue(steerValue, wheelIndex);

	if (missileActive)
		mMissile->Update();

	GameObject::Update();
}

void Car::InitRigidBody()
{
	//if mass is zero it counts as infinite
	float mass = 50.0f;

	btQuaternion q = OgreToBtQuaternion(mSceneNode->getOrientation());
	btVector3 v = OgreToBtVector3(mSceneNode->getPosition());
	//v.setY(v.getY() - 100);

	btTransform transform = btTransform(q, v);

	btCollisionShape* boxShape = new btBoxShape(btVector3(20, 5, 20));
	btCompoundShape* compound = new btCompoundShape();
	mCollisionShapes.push_back(boxShape);
	mCollisionShapes.push_back(compound);

	compound->addChildShape(transform, boxShape);

	btDefaultMotionState* boxMotionState = new btDefaultMotionState(transform);
	btVector3 boxInertia(0, 0, 0);
	boxShape->calculateLocalInertia(mass, boxInertia);
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass, boxMotionState, compound, boxInertia);
	
	mRigidBody = new btRigidBody(boxRigidBodyCI);
	mRigidBody->setActivationState(DISABLE_DEACTIVATION);
	world->addRigidBody(mRigidBody);

	

	
	float connectionHeight = 10.f;
	float wheelWidth = 4.f;
	float wheelRadius = 5.f;
	int rightIndex = 0;
	int upIndex = 1;
	int forwardIndex = 2;
	btVector3 wheelDirection(0, -1, 0);
	btVector3 wheelAxle(-1, 0, 0);
	float suspensionRestLength = 10.6f;
	bool isFrontWheel = true;

	btCollisionShape* wheelShape = new btCylinderShapeX(btVector3(wheelWidth, wheelRadius, wheelRadius));

	m_vehicleRayCaster = new btDefaultVehicleRaycaster(world);
	m_vehicle = new btRaycastVehicle(m_tuning, mRigidBody, m_vehicleRayCaster);

	world->addVehicle(m_vehicle);

	m_vehicle->setCoordinateSystem(rightIndex, upIndex, forwardIndex);
	btVector3 connectionPoint((10.f - 0.3f*wheelWidth), connectionHeight, (10.f - wheelRadius));
	m_vehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
	connectionPoint = btVector3((-10.f - 0.3f*wheelWidth), connectionHeight, (10.f - wheelRadius));
	m_vehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
	connectionPoint = btVector3((-10.f - 0.3f*wheelWidth), connectionHeight, (-10.f - wheelRadius));
	m_vehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, false);
	connectionPoint = btVector3((10.f - 0.3f*wheelWidth), connectionHeight, (-10.f - wheelRadius));
	m_vehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, false);

	float   wheelFriction = 1000;
	float   suspensionStiffness = 20.f;
	float   suspensionDamping = 2.3f;
	float   suspensionCompression = 4.4f;
	float   rollInfluence = 0.1f;


	for (int i = 0; i < m_vehicle->getNumWheels(); ++i){
		btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = suspensionStiffness;
		wheel.m_wheelsDampingRelaxation = suspensionDamping;
		wheel.m_wheelsDampingCompression = suspensionCompression;
		wheel.m_frictionSlip = wheelFriction;
		wheel.m_rollInfluence = rollInfluence;
	}
	
}