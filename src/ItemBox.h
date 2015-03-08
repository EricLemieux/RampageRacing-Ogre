#pragma once

#include "GameObject.h"

enum ITEM_BOX_TYPE
{
	BT_NONE,
	BT_ATTACK,
	BT_DEFENCE,
	BT_SPEED
};

class ItemBox :public GameObject
{
public:
	ItemBox(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, ITEM_BOX_TYPE type, Ogre::Vector3 position, Ogre::Quaternion rotation);
	~ItemBox();

	virtual void Update();

	virtual ITEM_BOX_TYPE getType(){ return mType; }

private:
	virtual void InitRigidBody();

	ITEM_BOX_TYPE mType;
};