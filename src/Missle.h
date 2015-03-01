#pragma once

#include "GameObject.h"

class Missle:public GameObject
{
public:
	Missle(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, Ogre::Entity* missleEnt);
	~Missle();

	virtual void Update();

	inline void setVelocity(float x, float y, float z){ mVelocity = btVector3(x,y,z); }

private:
	virtual void InitRigidBody();

public:
	btVector3 mVelocity;
};