#include "Mine.h"

Mine::Mine(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, Ogre::SceneNode* parent, btRigidBody* ownerBody)
{
	mName = name;

	mSceneManager = manager;

	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* missileEnt = mSceneManager->createEntity("Mine.mesh");
	mSceneNode->attachObject(missileEnt);

	mSceneNode->setPosition(parent->getPosition());
	mSceneNode->setOrientation(parent->getOrientation());

	objectType = MINE;
	ownerID = ownerBody;

	InitRigidBody();
}

Mine::~Mine()
{
}

void Mine::InitRigidBody()
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

void Mine::Update()
{
	GameObject::Update();
}