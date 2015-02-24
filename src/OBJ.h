#pragma once

#include <vector>

#include <Ogre.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

class OBJ
{
public:
	OBJ();
	OBJ(Ogre::String fileName);
	~OBJ();

	void Load(Ogre::String fileName);

	//We really only need the vertex data for what we are doing, so fuck it only going to collect that
	std::vector<btVector3> mVerts;
};