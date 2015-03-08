#include "ItemBox.h"

ItemBox::ItemBox(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, ITEM_BOX_TYPE type, Ogre::Vector3 position, Ogre::Quaternion rotation)
{
	mName = name;

	mType = type;

	mSceneManager = manager;

	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* missileEnt = mSceneManager->createEntity("ItemBox.mesh");
	mSceneNode->attachObject(missileEnt);

	mSceneNode->setPosition(position);
	mSceneNode->setOrientation(rotation);

	InitRigidBody();
}

ItemBox::~ItemBox()
{
}

void ItemBox::InitRigidBody()
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

	mRigidBody->setAngularFactor(0);
}

void ItemBox::Update()
{
	GameObject::Update();
}