#pragma once

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

class PhysicsWorld {
public:
	PhysicsWorld();
	~PhysicsWorld();

	void initWorld();
	void addBodyToWorld(btRigidBody* newBody);
	void updateWorld();

	void deleteWorld();

private:
	btDiscreteDynamicsWorld* world;

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