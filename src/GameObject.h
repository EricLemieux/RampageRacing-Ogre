#pragma once

#include <Ogre.h>

class GameObject
{
public:
	GameObject();
	~GameObject();
private:
	Ogre::SceneNode mSceneNode;
};