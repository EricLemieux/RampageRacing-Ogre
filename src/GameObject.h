#pragma once

#include <Ogre.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "System.h"

#include <memory>

class GameObject
{
public:
	GameObject(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager);
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

	std::shared_ptr<Ogre::SceneManager> mSceneManager;
};