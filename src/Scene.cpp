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
	currentSubMenu = nextSubMenu = sm_Main;

	deltaT = 0.0f;
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

	if (arg.key == OIS::KC_P)
	{
		if (currentSubMenu == sm_Main)
			nextSubMenu = sm_LevelSelect;
		else
			nextSubMenu = sm_Main;
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
	
	if (currentSubMenu != nextSubMenu)
	{
		if (deltaT < 1.0f)
		{
			GetCamera()->setPosition(Ogre::Math::lerp(GetCamPosFromSubMenu(currentSubMenu), GetCamTargetFromSubMenu(nextSubMenu), deltaT));
			GetCamera()->lookAt(Ogre::Math::lerp(GetCamTargetFromSubMenu(currentSubMenu), GetCamTargetFromSubMenu(nextSubMenu), deltaT));

			deltaT += 0.01f;
		}
		else
		{
			//Make sure its lined up properly
			GetCamera()->setPosition(GetCamPosFromSubMenu(nextSubMenu));
			GetCamera()->lookAt(GetCamTargetFromSubMenu(nextSubMenu));

			//Reset the variables so we can use them again
			currentSubMenu = nextSubMenu;
			deltaT = 0.0f;
		}
	}

	return true;
}

void MenuScene::LoadSceneFile(Ogre::String fileName)
{
	//Do the base scene stuff
	Scene::LoadSceneFile(fileName);

	//Set the position of the camera based on the starting camera node
	GetCamera()->setPosition(GetCamPosFromSubMenu(currentSubMenu));
	GetCamera()->lookAt(GetCamTargetFromSubMenu(currentSubMenu));
}

Ogre::Vector3 MenuScene::GetCamPosFromSubMenu(int subMenu)
{
	Ogre::String str = "camPos";
	char buf[50];
	sprintf_s(buf, "%i", subMenu);

	str += buf;

	return GetSceneManager()->getSceneNode(str)->getPosition();
}
Ogre::Vector3 MenuScene::GetCamTargetFromSubMenu(int subMenu)
{
	Ogre::String str = "camTar";
	char buf[50];
	sprintf_s(buf, "%i", subMenu);

	str += buf;

	return GetSceneManager()->getSceneNode(str)->getPosition();
}