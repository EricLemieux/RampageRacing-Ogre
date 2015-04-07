#include "ItemBox.h"

ItemBox::ItemBox(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, ITEM_BOX_TYPE type, Ogre::Vector3 position, Ogre::Quaternion rotation)
{
	mName = name;

	mType = type;

	mSceneManager = manager;

	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* itemBoxEnt = mSceneManager->createEntity("ItemBox.mesh");
	switch(type)
	{
	case IBT_ATTACK:
		mItemMaterialName = "hud_item_attack";
		itemBoxEnt->setMaterialName("itemBox_attack");
		break;
	case IBT_DEFENCE:
		mItemMaterialName = "hud_item_defend";
		itemBoxEnt->setMaterialName("itemBox_defend");
		break;
	case IBT_SPEED:
		mItemMaterialName = "hud_item_speed";
		itemBoxEnt->setMaterialName("itemBox_speed");
		break;
	default:
		itemBoxEnt->setMaterialName("itemBox_none");
		break;
	}
	mSceneNode->attachObject(itemBoxEnt);

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

	ghost = new btPairCachingGhostObject();
	ghost->setWorldTransform(transform);

	btCollisionShape* boxShape = new btBoxShape(btVector3(3, 10, 3));
	btDefaultMotionState* boxMotionState = new btDefaultMotionState(transform);
	btVector3 boxInertia(0, 0, 0);
	boxShape->calculateLocalInertia(mass, boxInertia);
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(0, boxMotionState, boxShape, boxInertia);
	mRigidBody = new btRigidBody(boxRigidBodyCI);
	mRigidBody->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);


	ghost->setCollisionShape(boxShape);
	ghost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

	mRigidBody->setAngularFactor(0);
}

void ItemBox::Update()
{
	ghost->setWorldTransform(mRigidBody->getWorldTransform());

	GameObject::Update();
}