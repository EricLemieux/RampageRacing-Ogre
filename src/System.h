#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <Ogre.h>

//Simple conversions
inline btQuaternion OgreToBtQuaternion(Ogre::Quaternion q){	return btQuaternion(q.x, q.y, q.z, q.w);	}
inline btVector3 OgreToBtVector3(Ogre::Vector3 v){	return btVector3(v.x, v.y, v.z);	}