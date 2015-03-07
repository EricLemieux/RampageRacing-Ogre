#pragma once

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>

#include <memory>

class PhysicsWorld {
public:
	PhysicsWorld();
	~PhysicsWorld();

	void initWorld();
	void addBodyToWorld(btRigidBody* newBody);
	void updateWorld(float timeStep);

	btDiscreteDynamicsWorld* getWorld(){ return world; }

	void deleteWorld();

private:
	btDiscreteDynamicsWorld* world;

	btAxisSweep3* sweepBP;
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;

	btSequentialImpulseConstraintSolver* solver;

	btCollisionShape* groundShape;
	btDefaultMotionState* groundMotionState;
	btRigidBody* groundRigidBody;
};

class PhysicsBody {
	void initCar();
	void initTrackSegment();
	btRigidBody* initBox(btScalar mass, btVector3& pos, btQuaternion& rot);
	void initSphere();

	btVector3 getPos();
	btQuaternion getQuatRot();

private:
	btCollisionShape* boxShape;
	btDefaultMotionState* boxMotionState;
	btRigidBody* boxRigidBody;
};