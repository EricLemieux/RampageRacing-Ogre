#pragma once

#include <Ogre.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>

#include "System.h"

#include <memory>

class GameObject
{
public:
	GameObject();
	GameObject(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager);
	~GameObject();

public:
	inline Ogre::SceneNode* GetSceneNode(void){return mSceneNode;}
	inline btRigidBody* GetRigidBody(void){return mRigidBody;}
	virtual void Update(void);

private:
	virtual void InitRigidBody();

protected:
	Ogre::String mName;
	Ogre::SceneNode* mSceneNode;
	btRigidBody* mRigidBody;

	std::shared_ptr<Ogre::SceneManager> mSceneManager;
};