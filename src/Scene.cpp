#include "Scene.h"

Scene::Scene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera)
{
	mSceneMgr = sceneMgr;

	mCamera = camera;

	ResetCamera();

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

void Scene::ResetCamera()
{
	mCamera->setPosition(0.0f, 0.0f, 0.0f);
	mCamera->lookAt(0.0f, 0.0f, -1.0f);
}


//Gameplay scenes
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
		mCar->mCanMoveForward = true;
	}
	else if (arg.key == OIS::KC_S)
	{
		mCar->mCanMoveBackward = true;
	}

	if (arg.key == OIS::KC_D)
	{
		mCar->TurnRight();
	}
	else if (arg.key == OIS::KC_A)
	{
		mCar->TurnLeft();
	}

	if (arg.key == OIS::KC_ESCAPE)
	{
		GetSceneManager()->clearScene();
		newScene = std::shared_ptr<MenuScene>(new MenuScene(GetSceneManager(), GetCamera()));
		newScene->LoadSceneFile("MainMenu.scene");
		swapToTheNewScene = true;
	}
}
void GameplayScene::KeyReleased(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_W)
	{
		mCar->mCanMoveForward = false;
	}
	if (arg.key == OIS::KC_S)
	{
		mCar->mCanMoveBackward = false;
	}

	if (arg.key == OIS::KC_D)
	{
		mCar->TurnRight(0.0f);
	}
	if (arg.key == OIS::KC_A)
	{
		mCar->TurnLeft(0.0f);
	}
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

	camNode->translate(Ogre::Vector3(0.0f, 10.0f, 40.0f));

	GetCamera()->lookAt(mCar->GetSceneNode()->getPosition());
}

bool GameplayScene::Update()
{
	GetPhysicsWorld()->updateWorld();

	mCar->Update();

	return true;
}


//Menu Scene
MenuScene::MenuScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera) : Scene(sceneMgr, camera)
{

}
MenuScene::~MenuScene()
{
}

void MenuScene::KeyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_SPACE)
	{
		GetSceneManager()->clearScene();
		newScene = std::shared_ptr<GameplayScene>(new GameplayScene(GetSceneManager(), GetCamera()));
		newScene->LoadSceneFile("test.scene");
		newScene->AddCarToScene("myCar");
		swapToTheNewScene = true;
	}

	if (arg.key == OIS::KC_ESCAPE)
	{
		//TODO Replace with an actual way to stop the program
		exit(1);
	}
}
void MenuScene::KeyReleased(const OIS::KeyEvent &arg)
{

}

bool MenuScene::Update()
{
	GetPhysicsWorld()->updateWorld();

	//GetSceneManager()->getSceneNode("Car")->rotate(Ogre::Vector3(0.0f, 1.0f, 0.0f), Ogre::Radian(0.001f));
	
	return true;
}