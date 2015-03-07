#pragma once

#include "GameObject.h"

class ItemBox :public GameObject
{
public:
	ItemBox(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, Ogre::SceneNode* parent);
	~ItemBox();

	virtual void Update();

private:
	virtual void InitRigidBody();
};