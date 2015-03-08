#pragma once

#include "GameObject.h"

class ItemBox :public GameObject
{
public:
	ItemBox(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, Ogre::SceneNode* parent);
	~ItemBox();

	virtual void Update();

	btPairCachingGhostObject* ghost;

private:
	virtual void InitRigidBody();
};