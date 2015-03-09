#pragma once

#include "GameObject.h"

enum ITEM_BOX_TYPE
{
	IBT_NONE,
	IBT_ATTACK,
	IBT_DEFENCE,
	IBT_SPEED
};

class ItemBox :public GameObject
{
public:
	ItemBox(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, ITEM_BOX_TYPE type, Ogre::Vector3 position, Ogre::Quaternion rotation);
	~ItemBox();

	virtual void Update();

	virtual ITEM_BOX_TYPE GetType(){ return mType; }

	virtual Ogre::String GetItemMaterialName(){ return mItemMaterialName; }

private:
	virtual void InitRigidBody();

	Ogre::String mItemMaterialName;

	ITEM_BOX_TYPE mType;
};