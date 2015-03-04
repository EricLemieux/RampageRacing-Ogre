#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <Ogre.h>

//Simple conversions

//From ogre to bullet
inline btQuaternion OgreToBtQuaternion(Ogre::Quaternion q){	return btQuaternion(q.x, q.y, q.z, q.w);	}
inline btVector3 OgreToBtVector3(Ogre::Vector3 v){	return btVector3(v.x, v.y, v.z);	}

//From bullet to ogre
inline Ogre::Quaternion BtToOgreQuaternion(btQuaternion *q){	return Ogre::Quaternion(q->w(), q->x(), q->y(), q->z());	}
inline Ogre::Vector3 BtToOgreVector3(btVector3 *v){	return Ogre::Vector3(v->x(), v->y(), v->z());	}