#include "GameObject.h"

GameObject::GameObject()
{
	name = "temp";
	mSceneNode = NULL;
}

GameObject::GameObject(Ogre::String name, Ogre::SceneNode* root)
{
	mSceneNode = root->createChildSceneNode(name);
}

GameObject::~GameObject()
{

}