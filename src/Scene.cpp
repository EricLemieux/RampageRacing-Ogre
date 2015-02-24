#include "Scene.h"

Scene::Scene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera, std::shared_ptr<Client> client)
{
	mSceneMgr = sceneMgr;

	mCamera = camera;

	ResetCamera();

	//Set up the physics world
	mPhysicsWorld = std::shared_ptr<PhysicsWorld>(new PhysicsWorld);
	mPhysicsWorld->initWorld();

	mSceneMgr->setSkyBox(true, "sky");

	mGameClient = client;
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

void Scene::LoadLevel(Ogre::String LevelName)
{
	Ogre::DotSceneLoader a;
	a.parseDotScene(LevelName + ".scene", "General", mSceneMgr.get());
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
void Scene::AddTriggerVolumesToScene()
{

}

void Scene::ResetCamera()
{
	mCamera->setPosition(0.0f, 0.0f, 0.0f);
	mCamera->lookAt(0.0f, 0.0f, -1.0f);
}


//Gameplay scenes
GameplayScene::GameplayScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera, std::shared_ptr<Client> client) : Scene(sceneMgr, camera, client)
{

}
GameplayScene::~GameplayScene()
{
}

void GameplayScene::LoadLevel(Ogre::String LevelName)
{
	Scene::LoadLevel(LevelName);

	//Load the physics mesh for that level
	OBJ physMesh(LevelName + ".obj");	

	btCollisionShape* boxShape = new btConvexHullShape(physMesh.mVerts[0], physMesh.mVerts.size(), sizeof(float));
	btDefaultMotionState* boxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));
	btVector3 boxInertia(0, 0, 0);
	boxShape->calculateLocalInertia(0.0f, boxInertia);
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(0.0f, boxMotionState, boxShape, boxInertia);
	body = new btRigidBody(boxRigidBodyCI);
	GetPhysicsWorld()->addBodyToWorld(body);
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
		newScene = std::shared_ptr<MenuScene>(new MenuScene(GetSceneManager(), GetCamera(), this->mGameClient));
		newScene->LoadLevel("MainMenu");
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
	mCar = new Car("mCar", GetSceneManager());

	//Add the car's rigid body to the world
	GetPhysicsWorld()->addBodyToWorld(mCar->GetRigidBody());

	Ogre::SceneNode* camNode = GetSceneManager()->getSceneNode("mCar")->createChildSceneNode();
	camNode->attachObject(GetCamera().get());
	
	camNode->translate(Ogre::Vector3(0.0f, 10.0f, 40.0f));

	GetCamera()->lookAt(mCar->GetSceneNode()->getPosition());

	callback = new ContactSensorCallback(*(mCar->GetRigidBody()), *(mCar));
}

void GameplayScene::AddTriggerVolumesToScene()
{
	//Temp only one trigger volume
	mTriggerVolumes[0] = new TriggerVolume("trigger", GetSceneManager());
	GetPhysicsWorld()->addBodyToWorld(mTriggerVolumes[0]->GetRigidBody());
}

bool GameplayScene::Update()
{
	GetPhysicsWorld()->updateWorld();
	GetPhysicsWorld()->getWorld()->contactPairTest(mCar->GetRigidBody(), mTriggerVolumes[0]->GetRigidBody(), *callback);
	bool test = mCar->isColliding;

	mCar->Update();

	//Send the position of the players car to the server
	{
		char str[256];
		auto pos = mCar->GetSceneNode()->getPosition();
		sprintf_s(str, 256, "%s %d %f %f %f", "pos", mGameClient->GetID(), pos.x, pos.y, pos.z);
		mGameClient->SendString(std::string(str));
	}

	//Send the rotation of the car to the server
	{
		char str[256];
		auto rot = mCar->GetSceneNode()->getOrientation();
		sprintf_s(str, 256, "%s %d %f %f %f", "rot", mGameClient->GetID(), rot.x, rot.y, rot.z);
		mGameClient->SendString(std::string(str));
	}
	
	//Get the positions from the server for the other cars
	mGameClient->Recieve();

	GetSceneManager()->getSceneNode("mCar2")->setPosition(mGameClient->GetPos(1));

	////TEMP AS FUCK
	////seriously kill this with fire...
	//if (test)
	//{
	//	GetSceneManager()->clearScene();
	//	newScene = std::shared_ptr<MenuScene>(new MenuScene(GetSceneManager(), GetCamera(), this->mGameClient));
	//	newScene->LoadLevel("MainMenu");
	//	swapToTheNewScene = true;
	//}

	return true;
}


//Menu Scene
MenuScene::MenuScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera, std::shared_ptr<Client> client) : Scene(sceneMgr, camera, client)
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
		if (currentSubMenu == sm_Main)
		{
			nextSubMenu = sm_LevelSelect;
		}
		else if (currentSubMenu == sm_LevelSelect)
		{
			GetSceneManager()->clearScene();
			newScene = std::shared_ptr<GameplayScene>(new GameplayScene(GetSceneManager(), GetCamera(), this->mGameClient));
			newScene->LoadLevel("test");
			newScene->AddCarToScene("myCar");
			newScene->AddTriggerVolumesToScene();
			swapToTheNewScene = true;
		}
	}

	if (arg.key == OIS::KC_ESCAPE)
	{
		if (currentSubMenu == sm_Main)
		{
			exit(1);
		}
		else if (currentSubMenu == sm_LevelSelect)
		{
			nextSubMenu = sm_Main;
		}
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
			if (currentSubMenu == sm_Main)
			{
				if (itr->movable->getName() == "bStartEnt0")
				{
					nextSubMenu = sm_LevelSelect;
				}
				else if (itr->movable->getName() == "bExitEnt0")
				{
					//exit the game

					exit(1);
				}
			}
			else if (currentSubMenu == sm_LevelSelect)
			{
				if (itr->movable->getName() == "bStartEnt1")
				{
					GetSceneManager()->clearScene();
					newScene = std::shared_ptr<GameplayScene>(new GameplayScene(GetSceneManager(), GetCamera(), this->mGameClient));
					newScene->LoadLevel("test");
					newScene->AddCarToScene("myCar");
					newScene->AddTriggerVolumesToScene();
					swapToTheNewScene = true;
				}
				else if (itr->movable->getName() == "bExitEnt1")
				{
					nextSubMenu = sm_Main;
				}
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

void MenuScene::LoadLevel(Ogre::String levelName)
{
	//Do the base scene stuff
	Scene::LoadLevel(levelName);

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