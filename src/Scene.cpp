#include "Scene.h"

Scene::Scene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera, std::shared_ptr<Client> client)
{
	mSceneMgr = sceneMgr;

	mCamera = camera;

	ResetCamera();

	//Set up the physics world
	mPhysicsWorld = std::shared_ptr<PhysicsWorld>(new PhysicsWorld);
	mPhysicsWorld->initWorld();

	mGameClient = client;
	timeStep = 0;
	clock.reset();
}

Scene::~Scene()
{
}

bool Scene::Update()
{
	timeStep += clock.getTimeSeconds();
	clock.reset();
	mPhysicsWorld->updateWorld(timeStep);

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

void Scene::axisMoved(const OIS::JoyStickEvent &arg, int axis)
{

}
void Scene::buttonPressed(const OIS::JoyStickEvent &arg, int button)
{

}
void Scene::buttonReleased(const OIS::JoyStickEvent &arg, int button)
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

void Scene::SwapToMainMenu()
{
	GetSceneManager()->clearScene();
	newScene = std::shared_ptr<MenuScene>(new MenuScene(GetSceneManager(), GetCamera(), this->mGameClient));
	newScene->LoadLevel("MainMenu");
	swapToTheNewScene = true;
}

void Scene::SwapToGameplayLevel(Ogre::String levelName)
{
	GetSceneManager()->clearScene();
	newScene = std::shared_ptr<GameplayScene>(new GameplayScene(GetSceneManager(), GetCamera(), this->mGameClient));
	newScene->LoadLevel(levelName);
	newScene->AddCarToScene("myCar");
	newScene->AddTriggerVolumesToScene();
	swapToTheNewScene = true;
}


//Gameplay scenes
GameplayScene::GameplayScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera, std::shared_ptr<Client> client) : Scene(sceneMgr, camera, client)
{
	//Set up common entitys
	mCommonMissile = mSceneMgr->createEntity("Missile", "Missile.mesh");
	mCommonMine = mSceneMgr->createEntity("mine", "Mine.mesh");
	//GetPhysicsWorld()->getWorld()->setInternalTickCallback((btInternalTickCallback)myTickCallback);
}
GameplayScene::~GameplayScene()
{
}

void GameplayScene::LoadLevel(Ogre::String levelName)
{
	Scene::LoadLevel(levelName);

	Ogre::String basename, path;
	Ogre::StringUtil::splitFilename(levelName + ".phys", basename, path);

	Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource(basename, "General");

	Ogre::String data;

	while (!pStream->eof())
	{
		//Get the next line of the file
		data = pStream->getLine();

		//Open the obj that file wants if it is not a comment
		if (data.c_str()[0] != '#')
		{
			//Load the physics mesh for that level
			OBJ physMesh(data);

			btCollisionShape* boxShape = new btBvhTriangleMeshShape(physMesh.mTriMesh, false);

			btDefaultMotionState* boxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
			btVector3 boxInertia(0, 0, 0);
			btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(0.0f, boxMotionState, boxShape, boxInertia);
			body = new btRigidBody(boxRigidBodyCI);
			GetPhysicsWorld()->addBodyToWorld(body);
		}
	}
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
		mCar->mTurningRight = true;
		//mCar->TurnRight();
	}
	else if (arg.key == OIS::KC_A)
	{
		mCar->mTurningLeft = true;
		//mCar->TurnLeft();
	}

	if (arg.key == OIS::KC_SPACE)
	{
		mCar->FireMissile(mCommonMissile);
	}

	if (arg.key == OIS::KC_P)
	{
		SwapToMainMenu();
	}

	if (arg.key == OIS::KC_ESCAPE)
	{
		SwapToMainMenu();
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
		mCar->mTurningRight = false;
	}
	if (arg.key == OIS::KC_A)
	{
		mCar->mTurningLeft = false;
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

void GameplayScene::axisMoved(const OIS::JoyStickEvent &arg, int axis)
{

}
void GameplayScene::buttonPressed(const OIS::JoyStickEvent &arg, int button)
{
	if (button == XBOX_A)
	{
		mCar->mCanMoveForward = true;
	}
	if (button == XBOX_B)
	{
		mCar->mCanMoveBackward = true;
	}
}
void GameplayScene::buttonReleased(const OIS::JoyStickEvent &arg, int button)
{
	if (button == XBOX_A)
	{
		mCar->mCanMoveForward = false;
	}
	if (button == XBOX_B)
	{
		mCar->mCanMoveBackward = false;
	}
}

void GameplayScene::AddCarToScene(Ogre::String name)
{
	//Create a game object thing
	mCar = new Car("mCar", GetSceneManager(), GetPhysicsWorld()->getWorld());

	//Add the car's rigid body to the world
//	GetPhysicsWorld()->addBodyToWorld(mCar->GetRigidBody());

	Ogre::SceneNode* camNode = GetSceneManager()->getSceneNode("mCar")->createChildSceneNode();
	camNode->attachObject(GetCamera().get());
	
	camNode->translate(Ogre::Vector3(0.0f, 10.0f, 40.0f));

	GetCamera()->lookAt(mCar->GetSceneNode()->getPosition());

	//Attach a light to the car
	Ogre::SceneNode* carLight = GetSceneManager()->getSceneNode("mCar")->createChildSceneNode();
	carLight->translate(0, 20, 0);
	Ogre::Light* carLightEnt = GetSceneManager()->createLight("carLight");
	carLightEnt->setDiffuseColour(1, 0, 0);
	carLight->attachObject(carLightEnt);

	//Attach the lap counter hud element
	Ogre::SceneNode* lapcounter = camNode->createChildSceneNode();
	lapcounter->translate(2.9, 0.4, -5);
	lapcounter->scale(0.5, 0.5, 0.5);
	Ogre::Entity* lapcounterEnt = mSceneMgr->createEntity("lapcounter", "button.mesh");
	lapcounterEnt->setMaterialName("hud_lap_1");
	lapcounter->attachObject(lapcounterEnt);

	callback = new ContactSensorCallback(*(mCar->GetRigidBody()), *(mCar));
}

void GameplayScene::AddTriggerVolumesToScene()
{
	//Temp only one trigger volume
	mTriggerVolumes[0] = new TriggerVolume("triggerFinishLine", GetSceneManager());
	GetPhysicsWorld()->addBodyToWorld(mTriggerVolumes[0]->GetRigidBody());

	mTriggerVolumes[1] = new TriggerVolume("triggerCheckpoint1", GetSceneManager());
	GetPhysicsWorld()->addBodyToWorld(mTriggerVolumes[1]->GetRigidBody());

	mTriggerVolumes[2] = new TriggerVolume("triggerCheckpoint2", GetSceneManager());
	GetPhysicsWorld()->addBodyToWorld(mTriggerVolumes[2]->GetRigidBody());
}

bool GameplayScene::Update()
{
	timeStep = clock.getTimeSeconds();
	clock.reset();
	GetPhysicsWorld()->updateWorld(timeStep);
	
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

	//GetSceneManager()->getSceneNode("mCar2")->setPosition(mGameClient->GetPos(1));

	if (mCar->checkPointsHit == 0)
	{
		GetPhysicsWorld()->getWorld()->contactPairTest(mCar->GetRigidBody(), mTriggerVolumes[1]->GetRigidBody(), *callback);
		if (mCar->isColliding)
		{
			mCar->isColliding = false;
			++mCar->checkPointsHit;
		}
	}
	else if (mCar->checkPointsHit == 1)
	{
		GetPhysicsWorld()->getWorld()->contactPairTest(mCar->GetRigidBody(), mTriggerVolumes[2]->GetRigidBody(), *callback);
		if (mCar->isColliding)
		{
			mCar->isColliding = false;
			++mCar->checkPointsHit;
		}
	}
	else if (mCar->checkPointsHit == 2)
	{
		GetPhysicsWorld()->getWorld()->contactPairTest(mCar->GetRigidBody(), mTriggerVolumes[0]->GetRigidBody(), *callback);
		if (mCar->isColliding)
		{
			mCar->isColliding = false;
			mCar->checkPointsHit = 0;

			if (mCar->lapCounter < 2)
			{
				++mCar->lapCounter;
				char buf[128];
				sprintf_s(buf,128,"%s%i","hud_lap_",mCar->lapCounter+1);
				GetSceneManager()->getEntity("lapcounter")->setMaterialName(buf);
			}
			else
			{
				SwapToMainMenu();
			}
		}
	}
	
	return true;
}

void GameplayScene::myTickCallback(btDynamicsWorld *world, btScalar timeStep){
	btVector3 speed = mCar->GetRigidBody()->getLinearVelocity();
	if (speed.norm() > 100){
		mCar->GetRigidBody()->applyCentralForce(-speed);
	}
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
			SwapToGameplayLevel("level1");
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
				Ogre::String str = itr->movable->getName();
				char buttonType[256], levelName[256];
				sscanf_s(str.c_str(), "%6s", &buttonType, 256);
				if (!stricmp(buttonType, "bLevel"))
				{
					//Move the current Selection back
					GetSceneManager()->getSceneNode(mCurrentSelectedLevel + "MenuMini")->setPosition(0.5, 40, 50);

					sscanf_s(str.c_str(),"%*[^_]_%s",&levelName,256);
					mCurrentSelectedLevel = levelName;
					GetSceneManager()->getSceneNode(Ogre::String(levelName) + "MenuMini")->setPosition(0.5, 40, -5);
				}

				else if (itr->movable->getName() == "bPlay")
				{
					if (mCurrentSelectedLevel != "")
					{
						SwapToGameplayLevel(mCurrentSelectedLevel);
					}
				}

				else if (itr->movable->getName() == "bHome")
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

void MenuScene::axisMoved(const OIS::JoyStickEvent &arg, int axis)
{
	if (arg.state.mAxes[0].rel > 0.5)
	{
		int a = 0;
	}

	//itJoystickListener = mJoystickListeners.begin();
	//itJoystickListenerEnd = mJoystickListeners.end();
	//for (; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener) {
	//	if (!itJoystickListener->second->axisMoved(e, axis))
	//		break;
	//}
}
void MenuScene::buttonPressed(const OIS::JoyStickEvent &arg, int button)
{
	if (button == XBOX_A)
	{
		if (currentSubMenu == sm_Main)
		{
			nextSubMenu = sm_LevelSelect;
		}
		else if (currentSubMenu == sm_LevelSelect)
		{
			SwapToGameplayLevel("level1");
		}
	}

	if (button == XBOX_B)
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
void MenuScene::buttonReleased(const OIS::JoyStickEvent &arg, int button)
{

}

bool MenuScene::Update()
{
	timeStep += clock.getTimeSeconds();
	clock.reset();
	GetPhysicsWorld()->updateWorld(timeStep);

	GetSceneManager()->getSceneNode("Car")->rotate(Ogre::Vector3(0.0f, 1.0f, 0.0f), Ogre::Radian(0.001f));

	if (mCurrentSelectedLevel != "")
	{
		GetSceneManager()->getSceneNode(mCurrentSelectedLevel + "MenuMini")->rotate(Ogre::Vector3(0.0f, 1.0f, 0.0f), Ogre::Radian(0.001f));
	}
	
	if (currentSubMenu != nextSubMenu)
	{
		if (deltaT < 1.0f)
		{
			GetCamera()->setPosition(Ogre::Math::lerp(GetCamPosFromSubMenu(currentSubMenu), GetCamTargetFromSubMenu(nextSubMenu), deltaT));
			GetCamera()->lookAt(Ogre::Math::lerp(GetCamTargetFromSubMenu(currentSubMenu), GetCamTargetFromSubMenu(nextSubMenu), deltaT));

			deltaT += 0.007f;
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

	//Set the default level
	mCurrentSelectedLevel = "Roadway";
	GetSceneManager()->getSceneNode(mCurrentSelectedLevel + "MenuMini")->setPosition(0.5, 40, -5);
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

//Intro Scene
IntroScene::IntroScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera, std::shared_ptr<Client> client) : Scene(sceneMgr, camera, client)
{

}

IntroScene::~IntroScene()
{

}

bool IntroScene::Update()
{
	mTime += clock.getTimeSeconds();
	clock.reset();

	if (mTime < 5.0f)
	{
		//Move the logo infront of the camera
		mSceneMgr->getSceneNode("UOIT_logo")->setPosition(0, 0, -5);
	}
	else if (mTime < 10.0f)
	{
		//Move the uoit logo out of the way and move the sleeples nights logo in front of the camera
		mSceneMgr->getSceneNode("UOIT_logo")->setPosition(0, 0, 50);
		mSceneMgr->getSceneNode("sleep_logo")->setPosition(0, 0, -5);
	}
	else
	{
		SwapToMainMenu();
	}

	Scene::Update();

	return true;
}

void IntroScene::LoadLevel(Ogre::String levelName)
{
	Scene::LoadLevel(levelName);

	//Set the position of the camera based on the starting camera node
	GetCamera()->setPosition(Ogre::Vector3(0,0,0));
	GetCamera()->lookAt(Ogre::Vector3(0,0,-5));
}