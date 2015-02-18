#include "Scene.h"

Scene::Scene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera)
{
	mSceneMgr = sceneMgr;

	mCamera = camera;

	ResetCamera();

	//Set up the physics world
	mPhysicsWorld = std::shared_ptr<PhysicsWorld>(new PhysicsWorld);
	mPhysicsWorld->initWorld();

	mSceneMgr->setSkyBox(true, "sky");
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
	Ogre::DotSceneLoader a;
	a.parseDotScene(fileName, "General", mSceneMgr.get());
}

void Scene::KeyPressed(const OIS::KeyEvent &arg)
{
	int a = 0;
}
void Scene::KeyReleased(const OIS::KeyEvent &arg)
{

}

void Scene::mouseMoved(const OIS::MouseEvent &arg)
{

}
void Scene::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{

}
void Scene::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
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

void GameplayScene::mouseMoved(const OIS::MouseEvent &arg)
{

}
void GameplayScene::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{

}
void GameplayScene::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{

}

void GameplayScene::AddCarToScene(Ogre::String name)
{
	//Create a game object thing
	mCar = new Car("myShip", GetSceneManager()->getSceneNode("mCar"));

	//Add the car's rigid body to the world
	GetPhysicsWorld()->addBodyToWorld(mCar->GetRigidBody());

	Ogre::SceneNode* camNode = GetSceneManager()->getSceneNode("mCar")->createChildSceneNode();
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

void MenuScene::mouseMoved(const OIS::MouseEvent &arg)
{

}
void MenuScene::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	Ogre::RaySceneQuery *q = GetSceneManager()->createRayQuery(Ogre::Ray());

	float x = arg.state.X.abs/1280.0f, y = arg.state.Y.abs/720.0f;

	Ogre::Ray ray = GetCamera()->getCameraToViewportRay(x,y);

	q->setRay(ray);
	q->setSortByDistance(true);

	Ogre::RaySceneQueryResult res = q->execute();
	Ogre::RaySceneQueryResult::iterator itr = res.begin();

	//Loop to select the object
	for (itr; itr != res.end(); itr++)
	{
		if (itr->movable && itr->movable->getName()[0] == 'b')
		{
			//We found the object
			if (itr->movable->getName() == "bStartEnt")
			{
				//Start the game

				GetSceneManager()->clearScene();
				newScene = std::shared_ptr<GameplayScene>(new GameplayScene(GetSceneManager(), GetCamera()));
				newScene->LoadSceneFile("test.scene");
				newScene->AddCarToScene("myCar");
				swapToTheNewScene = true;
			}
			else if (itr->movable->getName() == "bExitEnt")
			{
				//exit the game

				exit(1);
			}

			break;
		}
	}
}
void MenuScene::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{

}

bool MenuScene::Update()
{
	GetPhysicsWorld()->updateWorld();

	GetSceneManager()->getSceneNode("Car")->rotate(Ogre::Vector3(0.0f, 1.0f, 0.0f), Ogre::Radian(0.001f));
	
	return true;
}