#include "Scene.h"

Scene::Scene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera)
{
	mSceneMgr = sceneMgr;

	mCamera = camera;

	//Set up the physics world
	mPhysicsWorld = std::shared_ptr<PhysicsWorld>(new PhysicsWorld);
	mPhysicsWorld->initWorld();
}

Scene::~Scene()
{
}

bool Scene::Update()
{
	mPhysicsWorld->updateWorld();

	unsigned int size = mObjects.size(), i = 0;
	for (;i < size; ++i)
	{
		mObjects[i]->Update();
	}

	return true;
}

void Scene::ClearScene()
{
	//mSceneMgr->clearScene();
}

void Scene::LoadSceneFile(Ogre::String fileName)
{
	parseDotScene(fileName, "General", mSceneMgr.get());
}

void Scene::KeyPressed(const OIS::KeyEvent &arg)
{
	int a = 0;
}
void Scene::KeyReleased(const OIS::KeyEvent &arg)
{

}

void Scene::AddCarToScene(Ogre::String name)
{

}



GameplayScene::GameplayScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera) : Scene(sceneMgr, camera)
{

}
GameplayScene::~GameplayScene()
{
}

void GameplayScene::KeyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_W)
	{
		mCar->GetRigidBody()->translate(btVector3(0, 10, 0));
	}
}
void GameplayScene::KeyReleased(const OIS::KeyEvent &arg)
{

}

void GameplayScene::AddCarToScene(Ogre::String name)
{
	Ogre::SceneNode* controllerNode = GetSceneManager()->getRootSceneNode()->createChildSceneNode("controllerNode");

	//Create a game object thing
	mCar = new Car("myShip", controllerNode);
	Ogre::Entity* myShipEnt = GetSceneManager()->createEntity("shipEnt", "BoltCar.mesh");
	mCar->GetSceneNode()->attachObject(myShipEnt);

	//Add the car's rigid body to the world
	GetPhysicsWorld()->addBodyToWorld(mCar->GetRigidBody());

	Ogre::SceneNode* camNode = controllerNode->createChildSceneNode("camNode");
	camNode->attachObject(GetCamera().get());

	camNode->translate(Ogre::Vector3(0.0f, 35.0f, 7.0f));

	GetCamera()->lookAt(mCar->GetSceneNode()->getPosition());
}

bool GameplayScene::Update()
{
	mCar->Update();

	return true;
}