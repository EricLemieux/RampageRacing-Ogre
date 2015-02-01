#pragma once

#include <Ogre.h>

class GameObject
{
public:
	GameObject();
	GameObject(Ogre::String name, Ogre::SceneNode* root);
	~GameObject();

public:
	inline Ogre::SceneNode* GetSceneNode(void){return mSceneNode;}
private:
	Ogre::String name;
	Ogre::SceneNode* mSceneNode;
};