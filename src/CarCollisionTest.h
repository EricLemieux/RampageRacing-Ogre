#pragma once

#include "Car.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>


struct ContactSensorCallback : public btCollisionWorld::ContactResultCallback {



	ContactSensorCallback(btRigidBody& tgtBody, Car& mCar)
		: btCollisionWorld::ContactResultCallback(), body(tgtBody), car(mCar){ }

	btRigidBody& body;
	Car& car;


	virtual bool needsCollision(btBroadphaseProxy* proxy) const;

	virtual btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0, int, int,
		const btCollisionObjectWrapper*, int, int);
};