#pragma once

#include <Ogre.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "System.h"

class GameObject
{
public:
	GameObject();
	GameObject(Ogre::String name, Ogre::SceneNode* node);
	~GameObject();

public:
	inline Ogre::SceneNode* GetSceneNode(void){return mSceneNode;}
	inline btRigidBody* GetRigidBody(void){return mRigidBody;}
	virtual void Update(void);

private:
	void InitRigidBody();

private:
	Ogre::String mName;
	Ogre::SceneNode* mSceneNode;
	btRigidBody* mRigidBody;
};