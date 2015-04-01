#pragma once

#include <Ogre.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>

#include "System.h"

#include <memory>

static const Ogre::int32 masks[4] = { 0x0000000F, 0x000000F0, 0x00000F00, 0x0000F000 };
static const Ogre::int32 RenderOnly[4] = { 0x0000000F, 0x000000F0, 0x00000F00, 0x0000F000 };

enum OBJECT_TYPE{
	CAR = 0,
	MISSILE = 1,
	MINE = 2
};

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
	btPairCachingGhostObject* ghost;
	btRigidBody* ownerID;
	int lifeTimer;
	OBJECT_TYPE objectType;
	bool isActive;

	inline void SetPosition(Ogre::Vector3 pos){ mSceneNode->setPosition(pos); mRigidBody->getWorldTransform().setOrigin(OgreToBtVector3(pos)); }
	inline void SetRotation(Ogre::Quaternion rot){ mSceneNode->setOrientation(rot); mRigidBody->getWorldTransform().setRotation(OgreToBtQuaternion(rot)); }

private:
	virtual void InitRigidBody();

protected:
	int id;

	Ogre::String mName;
	Ogre::SceneNode* mSceneNode;
	btRigidBody* mRigidBody;

	std::shared_ptr<Ogre::SceneManager> mSceneManager;
};