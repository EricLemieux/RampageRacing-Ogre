#include "TriggerVolume.h"

TriggerVolume::TriggerVolume(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager)
{
	mName = name;
	mSceneManager = manager;
	mSceneNode = mSceneManager->getSceneNode(name);

	btTransform transform = btTransform(OgreToBtQuaternion(mSceneNode->getOrientation()), OgreToBtVector3(mSceneNode->getPosition()));

	OBJ physMesh("triggerVolume.obj");
	btCollisionShape* boxShape = new btBvhTriangleMeshShape(physMesh.mTriMesh, false);
			
	btDefaultMotionState* boxMotionState = new btDefaultMotionState(transform);
	btVector3 boxInertia(0, 0, 0);
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(0, boxMotionState, boxShape, boxInertia);
	mRigidBody = new btRigidBody(boxRigidBodyCI);
	mRigidBody->setCollisionFlags(btRigidBody::CF_NO_CONTACT_RESPONSE);
}

TriggerVolume::~TriggerVolume()
{

}