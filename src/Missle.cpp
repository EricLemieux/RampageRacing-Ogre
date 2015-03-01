#include "Missle.h"

Missle::Missle(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, Ogre::Entity* missleEnt, Ogre::SceneNode* parent)
{
	mName = name;

	mSceneManager = manager;

	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode("missile");
	mSceneNode->attachObject(missleEnt);

	mSceneNode->setPosition(parent->getPosition());
	mSceneNode->setOrientation(parent->getOrientation());

	InitRigidBody();
}

Missle::~Missle()
{

}

void Missle::InitRigidBody()
{
	//if mass is zero it counts as infinite
	float mass = 1.0f;

	btQuaternion q = OgreToBtQuaternion(mSceneNode->getOrientation());
	btVector3 v = OgreToBtVector3(mSceneNode->getPosition());

	btTransform transform = btTransform(q, v);

	btCollisionShape* boxShape = new btBoxShape(btVector3(1, 1, 1));
	btDefaultMotionState* boxMotionState = new btDefaultMotionState(transform);
	btVector3 boxInertia(0, 0, 0);
	boxShape->calculateLocalInertia(mass, boxInertia);
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass, boxMotionState, boxShape, boxInertia);
	mRigidBody = new btRigidBody(boxRigidBodyCI);
}

void Missle::Update()
{
	mRigidBody->setLinearVelocity(mVelocity);

	GameObject::Update();
}