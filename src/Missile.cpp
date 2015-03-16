#include "Missile.h"

Missile::Missile(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, Ogre::SceneNode* parent, btRigidBody* carID)
{
	mName = name;

	mSceneManager = manager;

	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* missileEnt = mSceneManager->createEntity("Missile.mesh");
	mSceneNode->attachObject(missileEnt);

	mSceneNode->setPosition(parent->getPosition());
	mSceneNode->setOrientation(parent->getOrientation());

	ownerID = carID;
	objectType = MISSILE;
	lifeTimer = 0;

	InitRigidBody();
}

Missile::~Missile()
{
}

void Missile::InitRigidBody()
{
	//if mass is zero it counts as infinite
	float mass = 1.0f;

	btQuaternion q = OgreToBtQuaternion(mSceneNode->getOrientation());
	btVector3 v = OgreToBtVector3(mSceneNode->getPosition());

	btTransform transform = btTransform(q, v);

	ghost = new btPairCachingGhostObject();
	ghost->setWorldTransform(transform);

	btCollisionShape* boxShape = new btBoxShape(btVector3(1, 1, 1));
	btDefaultMotionState* boxMotionState = new btDefaultMotionState(transform);
	btVector3 boxInertia(0, 0, 0);
	boxShape->calculateLocalInertia(mass, boxInertia);
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass, boxMotionState, boxShape, boxInertia);
	mRigidBody = new btRigidBody(boxRigidBodyCI);

	ghost->setCollisionShape(boxShape);
	ghost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

	mRigidBody->setAngularFactor(0);
}

void Missile::Update()
{
	mRigidBody->applyCentralForce(mVelocity);
	ghost->setWorldTransform(mRigidBody->getWorldTransform());
	lifeTimer++;
	GameObject::Update();
}