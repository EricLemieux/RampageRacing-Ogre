#include "Physics.h"

PhysicsWorld::PhysicsWorld(){
	 
}
PhysicsWorld::~PhysicsWorld(){}

void PhysicsWorld::initWorld(){
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();

    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	//gravity set here
	world->setGravity(btVector3(0,-10,0));

	//set floor as horizontal plane centered at 0,0,0
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    world->addRigidBody(groundRigidBody);
}

void PhysicsWorld::addBodyToWorld(btRigidBody* newBody){
	world->addRigidBody(newBody);
}

void PhysicsWorld::updateWorld(){
	world->stepSimulation(1.f / 60.0f, 0);
}

void PhysicsWorld::deleteWorld(){
	//TODO clean up all those pointers you have lying around
}

void PhysicsBody::initCar(){}//TODO
void PhysicsBody::initTrackSegment(){}//TODO
btRigidBody* PhysicsBody::initBox(btScalar mass, btVector3& pos, btQuaternion& rot){
	//if mass is zero it counts as infinite
	btCollisionShape* boxShape = new btBoxShape(btVector3(1,1,1));
	btDefaultMotionState* boxMotionState = new btDefaultMotionState(btTransform(rot,pos));
	btVector3 boxInertia(0,0,0);
	boxShape->calculateLocalInertia(mass, boxInertia);
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass,boxMotionState,boxShape,boxInertia);
	btRigidBody* boxRigidBody = new btRigidBody(boxRigidBodyCI);

	return boxRigidBody;
}
void PhysicsBody::initSphere(){}

