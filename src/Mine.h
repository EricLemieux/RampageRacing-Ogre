#pragma once

#include "GameObject.h"

class Mine :public GameObject
{
public:
	Mine(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, Ogre::SceneNode* parent);
	~Mine();

	virtual void Update();

private:
	virtual void InitRigidBody();
};