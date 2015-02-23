#include "CarCollisionTest.h"

bool ContactSensorCallback::needsCollision(btBroadphaseProxy* proxy) const {

	if (!btCollisionWorld::ContactResultCallback::needsCollision(proxy))
		return false;

	return body.checkCollideWithOverride(static_cast<btCollisionObject*>(proxy->m_clientObject));
}

btScalar ContactSensorCallback::addSingleResult(btManifoldPoint& cp,
	const btCollisionObjectWrapper* colObj0, int, int,
	const btCollisionObjectWrapper*, int, int)
{
	btVector3 pt;
	if (colObj0->m_collisionObject == &body) {
		pt = cp.m_localPointA;
		car.isColliding = true;
	}
	else {

		pt = cp.m_localPointB;
		car.isColliding = false;
	}

	return 0;
}