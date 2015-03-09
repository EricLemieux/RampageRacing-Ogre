#include "Scene.h"

Scene::Scene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Client> client, std::shared_ptr<Ogre::Camera> cameras[4], std::shared_ptr<Ogre::RenderWindow> window, std::shared_ptr<Controller> controllers[4])
{
	mSceneMgr = sceneMgr;

	mCamera = cameras[0];

	for (unsigned int i = 0; i < 4; ++i)
	{
		mCameras[i] = cameras[i];
		mControllers[i] = controllers[i];
	}

	ResetCamera();
	
	//Set up the physics world
	mPhysicsWorld = std::shared_ptr<PhysicsWorld>(new PhysicsWorld);
	mPhysicsWorld->initWorld();

	mWindow = window;

	mWindow->setActive(true);

	//mCarEnt = std::shared_ptr<Ogre::Entity>(mSceneMgr->createEntity("BoltCar", "BoltCar.mesh"));

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

	ControllerInput();

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

void Scene::KeyPressed(const OIS::KeyEvent &arg){}
void Scene::KeyReleased(const OIS::KeyEvent &arg){}

void Scene::mouseMoved(const OIS::MouseEvent &arg){}
void Scene::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id){}
void Scene::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id){}

void Scene::AddCarToScene(Ogre::String name){}
void Scene::AddTriggerVolumesToScene(){}

void Scene::ResetCamera()
{
	for (unsigned int i = 0; i < numLocalPlayers; ++i)
	{
		mCameras[i]->setPosition(0.0f, 0.0f, 0.0f);
		mCameras[i]->lookAt(0.0f, 0.0f, -1.0f);
		mCameras[i]->setNearClipDistance(0.1f);
	}
}

void Scene::SwapToMainMenu()
{
	GetSceneManager()->clearScene();
	newScene = std::shared_ptr<MenuScene>(new MenuScene(mSceneMgr, this->mGameClient, mCameras, mWindow, mControllers));
	newScene->LoadLevel("MainMenu");
	swapToTheNewScene = true;
}

void Scene::SwapToGameplayLevel(Ogre::String levelName)
{
	GetSceneManager()->clearScene();
	newScene = std::shared_ptr<GameplayScene>(new GameplayScene(mSceneMgr, this->mGameClient, mCameras, mWindow, mControllers));
	newScene->LoadLevel(levelName);
	newScene->AddCarToScene("myCar");
	newScene->AddTriggerVolumesToScene();
	swapToTheNewScene = true;
}

void Scene::SetUpViewports()
{
	//Clear all of the viewports
	mWindow->removeAllViewports();

	// Create one viewport, entire window
	Ogre::Viewport* vp = mWindow->addViewport(mCamera.get());
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void Scene::ControllerInput()
{

}


//Gameplay scenes
GameplayScene::GameplayScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Client> client, std::shared_ptr<Ogre::Camera> cameras[4], std::shared_ptr<Ogre::RenderWindow> window, std::shared_ptr<Controller> controllers[4]) : Scene(sceneMgr, client, cameras, window, controllers)
{
	SetUpViewports();
}
GameplayScene::~GameplayScene(){}

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

	SetUpItemBoxes();
}

void GameplayScene::SetUpItemBoxes()
{
	//should be replaced with the number of checkpoints, currently its only 3 so whatever
	//TODO
	for (unsigned int i = 1; i < 3; ++i)
	{
		for (unsigned int j = 0; j < 3; ++j)
		{
			char name[128];
			sprintf_s(name, 128, "itemBox%i", (i*3)+j);

			Ogre::Vector3 pos;
			Ogre::Quaternion rot;

			char checkpointName[128];
			sprintf_s(checkpointName, 128, "triggerCheckpoint%i", i);
			Ogre::Matrix4 m = mSceneMgr->getSceneNode(checkpointName)->_getFullTransform();
			Ogre::Vector3 rightVector = Ogre::Vector3(m[0][0], m[0][1], m[0][2]).normalisedCopy();

			pos = mSceneMgr->getSceneNode(checkpointName)->getPosition();
			pos += rightVector * (30 - j * 30.0f);
			pos.y -= 5.0f;
			rot = mSceneMgr->getSceneNode(checkpointName)->getOrientation();

			ITEM_BOX_TYPE boxType = IBT_NONE;
			if (j == 0)
				boxType = IBT_ATTACK;
			else if (j == 1)
				boxType = IBT_DEFENCE;
			else if (j == 2)
				boxType = IBT_SPEED;

			std::shared_ptr<ItemBox> box = std::shared_ptr<ItemBox>(new ItemBox(name, mSceneMgr, boxType, pos, rot));
			mPhysicsWorld->getWorld()->addRigidBody(box->GetRigidBody());
			mPhysicsWorld->getWorld()->addCollisionObject(box->ghost);

			mItemBoxes.push_back(box);

			
		}
	}
}

void GameplayScene::KeyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_W)
	{
		mCar->mCanMoveForward = 1.0f;
	}
	else if (arg.key == OIS::KC_S)
	{
		mCar->mCanMoveBackward = 1.0f;
	}

	if (arg.key == OIS::KC_D)
	{
		mCar->mTurning = 1.0f;
	}
	else if (arg.key == OIS::KC_A)
	{
		mCar->mTurning = -1.0f;
	}

	if (arg.key == OIS::KC_SPACE)
	{
		UseItem(0);
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
		mCar->mCanMoveForward = 0.0f;
	}
	if (arg.key == OIS::KC_S)
	{
		mCar->mCanMoveBackward = 0.0f;
	}

	if (arg.key == OIS::KC_D)
	{
		mCar->mTurning = 0.0f;
	}
	if (arg.key == OIS::KC_A)
	{
		mCar->mTurning = 0.0f;
	}
}

void GameplayScene::mouseMoved(const OIS::MouseEvent &arg){}
void GameplayScene::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id){}
void GameplayScene::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id){}

void GameplayScene::AddCarToScene(Ogre::String name)
{
	for (unsigned int i = 0; i < numLocalPlayers; ++i)
	{
		char name[128];
		sprintf_s(name, 128, "mCar%i",i);

		//Create a game object thing
		mCars[i] = std::shared_ptr<Car>(new Car(name, GetSceneManager(), GetPhysicsWorld()->getWorld(), "BoltCar.mesh", i));

		Ogre::SceneNode* camNode = GetSceneManager()->getSceneNode(name)->createChildSceneNode();
		camNode->attachObject(mCameras[i].get());

		camNode->translate(Ogre::Vector3(0.0f, 10.0f, 40.0f));

		mCameras[i]->lookAt(mCars[i]->GetSceneNode()->getPosition());

		//Attach a light to the car
		Ogre::SceneNode* carLight = mSceneMgr->getSceneNode(name)->createChildSceneNode();
		carLight->translate(0, 20, 0);
		char l[128];
		sprintf_s(l, 128, "carLight%i", i);
		Ogre::Light* carLightEnt = mSceneMgr->createLight(l);
		carLightEnt->setDiffuseColour(1, 1, 1);
		carLight->attachObject(carLightEnt);

		//Set up the HUD elements attached to the car
		Ogre::SceneNode* lapCounterNode = mSceneMgr->getSceneNode(name)->createChildSceneNode();
		lapCounterNode->translate(0, 0, 20);
		char cw[128];
		sprintf_s(cw, 128, "currentWeapon%i", i);
		Ogre::Entity* lapCounterEnt = mSceneMgr->createEntity(cw,"HUDTile.mesh");
		lapCounterEnt->setMaterialName("hud_empty");
		lapCounterNode->attachObject(lapCounterEnt);

		Ogre::SceneNode* thingNode = mSceneMgr->getSceneNode(name)->createChildSceneNode();
		thingNode->translate(-3, -1.2, 20);
		thingNode->scale(0.6, 0.6, 0.6);
		char pp[128];
		sprintf_s(pp, 128, "playerPostion%i", i);
		Ogre::Entity* thingEnt = mSceneMgr->createEntity(pp, "HUDTile.mesh");
		thingEnt->setMaterialName("hud_empty");
		thingNode->attachObject(thingEnt);

		Ogre::SceneNode* thing2Node = mSceneMgr->getSceneNode(name)->createChildSceneNode();
		thing2Node->translate(3, -1.2, 20);
		thing2Node->scale(0.6, 0.6, 0.6);
		char lc[128];
		sprintf_s(lc, 128, "lapCounter%i", i);
		Ogre::Entity* thing2Ent = mSceneMgr->createEntity(lc, "HUDTile.mesh");
		thing2Ent->setMaterialName("hud_lap_1");
		thing2Node->attachObject(thing2Ent);

		callback = new ContactSensorCallback(*(mCars[i]->GetRigidBody()), *(mCars[i]));
	}

	mCar = mCars[0];	
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

	ControllerInput();
	
	for (unsigned int i = 0; i < numLocalPlayers; ++i)
	{
		mCars[i]->Update();
	}

	//update the active weapons
	std::for_each(mActiveWeapons.begin(), mActiveWeapons.end(), [](std::shared_ptr<GameObject> obj){obj->Update(); });

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

	//COLLISION DETECTION
	btDiscreteDynamicsWorld* world = GetPhysicsWorld()->getWorld();

	for (int i = 0; i < numLocalPlayers; ++i){
		world->getDispatcher()->dispatchAllCollisionPairs(mCars[i]->ghost->getOverlappingPairCache(), world->getDispatchInfo(), world->getDispatcher());
		//btTransform transform = mCar->ghost->getWorldTransform();
		btManifoldArray manifoldArray;

		for (int j = 0; j < mCars[i]->ghost->getOverlappingPairCache()->getNumOverlappingPairs(); ++j){
			manifoldArray.resize(0);
			btBroadphasePair* collisionPair = &(mCars[i]->ghost->getOverlappingPairCache()->getOverlappingPairArray()[j]);
			if (collisionPair->m_algorithm)
				collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

			//for (int k = 0; k < manifoldArray.size(); ++k){
				if (manifoldArray.size() > 0){
					btPersistentManifold* manifold = manifoldArray[0];

					//HARD CODED CHECK POINT NUMBERS, FIX THIS AT SOME POINT
					for (unsigned int tv = 0; tv < 3; ++tv){
						if (manifold->getBody1() == mTriggerVolumes[tv]->GetRigidBody()){
							if (mCars[i]->checkPointsHit == 0 && tv == 1){
								mCars[i]->checkPointsHit++;
							}
							else if (mCars[i]->checkPointsHit == 1 && tv == 2){
								mCars[i]->checkPointsHit++;
							}
							else if (mCars[i]->checkPointsHit == 2 && tv == 0){
								mCars[i]->checkPointsHit = 0;
								mCars[i]->lapCounter++;

								if (mCars[i]->lapCounter > 2)
								{
									if (mNumPlayersCompletedRace == numLocalPlayers-1)
									{
										SwapToMainMenu();
									}
									else
									{
										mCars[i]->mFinishedRace = true;
										mNumPlayersCompletedRace++;
									}
								}
								else
								{
									char lc[128];
									sprintf_s(lc, 128, "lapCounter%i", i);

									char m[128];
									sprintf_s(m, 128, "hud_lap_%i", mCars[i]->lapCounter+1);

									mSceneMgr->getEntity(lc)->setMaterialName(m);
								}
							}
						}
					}

					for (unsigned int ib = 0; ib < mItemBoxes.size(); ++ib)
					{
						bool test0 = manifold->getBody0() == mItemBoxes[ib]->GetRigidBody() ? true : false;
						bool test1 = manifold->getBody1() == mItemBoxes[ib]->GetRigidBody() ? true : false;
						if ((test0 || test1) && mCars[i]->mCurrentItem == IBT_NONE)
						{
							mCars[i]->mCurrentItem = mItemBoxes[ib]->GetType();

							char cw[128];
							sprintf_s(cw, 128, "currentWeapon%i", i);
							mSceneMgr->getEntity(cw)->setMaterialName(mItemBoxes[ib]->GetItemMaterialName());
						}
					}

					std::list<std::shared_ptr<GameObject>>::iterator itr = mActiveWeapons.begin();
					for (; itr != mActiveWeapons.end(); ++itr){
						if (manifold->getBody0() == (*itr)->GetRigidBody() || manifold->getBody1() == (*itr)->GetRigidBody()){
							btVector3 force = (*itr)->GetRigidBody()->getWorldTransform().getOrigin() - mCars[i]->GetRigidBody()->getWorldTransform().getOrigin();
							//HOTFIX FOR PRESENTATION
							//mCars[i]->GetRigidBody()->applyCentralImpulse(force * 1000);
						}
					}
				}
			//}
		}
	}

	std::for_each(mActiveWeapons.begin(), mActiveWeapons.end(), [&](std::shared_ptr<GameObject> obj) {
		world->getDispatcher()->dispatchAllCollisionPairs(obj->ghost->getOverlappingPairCache(), world->getDispatchInfo(), world->getDispatcher());
		//btTransform transform = mCar->ghost->getWorldTransform();
		btManifoldArray manifoldArray;

		for (int i = 0; i < obj->ghost->getOverlappingPairCache()->getNumOverlappingPairs(); ++i){
			manifoldArray.resize(0);
			btBroadphasePair* collisionPair = &(obj->ghost->getOverlappingPairCache()->getOverlappingPairArray()[i]);
			if (collisionPair->m_algorithm)
				collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

			//for (int j = 0; j < manifoldArray.size(); ++j){
			if (manifoldArray.size() > 0){
				btPersistentManifold* manifold = manifoldArray[0];
				bool test0 = manifold->getBody1() == mTriggerVolumes[1]->GetRigidBody() ? true : false;
				bool test1 = manifold->getBody1() == mTriggerVolumes[1]->GetRigidBody() ? true : false;
				if (test1 || test0)
 					int a = 0;
			}
			//}
		}
	});	

	return true;
}

void GameplayScene::myTickCallback(btDynamicsWorld *world, btScalar timeStep){
	btVector3 speed = mCar->GetRigidBody()->getLinearVelocity();
	if (speed.norm() > 100){
		mCar->GetRigidBody()->applyCentralForce(-speed);
	}
}

void GameplayScene::SetUpViewports()
{
	//Clear all of the viewports
	mWindow->removeAllViewports();


	switch (numLocalPlayers)
	{
	case 1:
	{
		Ogre::Viewport* vp = mWindow->addViewport(mCamera.get());
		mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
		break;
	}
	case 2:
	{
		Ogre::Viewport* vp1 = mWindow->addViewport(mCamera.get());
		vp1->setDimensions(0, 0, 1, 0.5);
		mCamera->setAspectRatio(Ogre::Real(vp1->getActualWidth()) / Ogre::Real(vp1->getActualHeight()));
		vp1->setOverlaysEnabled(false);
	
		Ogre::Viewport* vp2 = mWindow->addViewport(mCameras[1].get(), 1);
		vp2->setDimensions(0, 0.5, 1, 0.5);
		mCameras[1]->setAspectRatio(Ogre::Real(vp2->getActualWidth()) / Ogre::Real(vp2->getActualHeight()));
		break;
	}
	case 3:
	{
		Ogre::Viewport* vp1 = mWindow->addViewport(mCamera.get());
		vp1->setDimensions(0, 0, 0.5, 0.5);
		mCamera->setAspectRatio(Ogre::Real(vp1->getActualWidth()) / Ogre::Real(vp1->getActualHeight()));
	
		Ogre::Viewport* vp2 = mWindow->addViewport(mCameras[1].get(), 1);
		vp2->setDimensions(0.5, 0, 0.5, 0.5);
		mCameras[1]->setAspectRatio(Ogre::Real(vp2->getActualWidth()) / Ogre::Real(vp2->getActualHeight()));
	
		Ogre::Viewport* vp3 = mWindow->addViewport(mCameras[2].get(), 2);
		vp3->setDimensions(0, 0.5, 1, 0.5);
		mCameras[2]->setAspectRatio(Ogre::Real(vp3->getActualWidth()) / Ogre::Real(vp3->getActualHeight()));
		break;
	}
	case 4:
	{
		Ogre::Viewport* vp1 = mWindow->addViewport(mCamera.get());
		vp1->setDimensions(0, 0, 0.5, 0.5);
		mCamera->setAspectRatio(Ogre::Real(vp1->getActualWidth()) / Ogre::Real(vp1->getActualHeight()));
	
		Ogre::Viewport* vp2 = mWindow->addViewport(mCameras[1].get(), 1);
		vp2->setDimensions(0.5, 0, 0.5, 0.5);
		mCameras[1]->setAspectRatio(Ogre::Real(vp2->getActualWidth()) / Ogre::Real(vp2->getActualHeight()));
	
		Ogre::Viewport* vp3 = mWindow->addViewport(mCameras[2].get(), 2);
		vp3->setDimensions(0, 0.5, 0.5, 0.5);
		mCameras[2]->setAspectRatio(Ogre::Real(vp3->getActualWidth()) / Ogre::Real(vp3->getActualHeight()));
	
		Ogre::Viewport* vp4 = mWindow->addViewport(mCameras[3].get(), 3);
		vp4->setDimensions(0.5, 0.5, 0.5, 0.5);
		mCameras[3]->setAspectRatio(Ogre::Real(vp4->getActualWidth()) / Ogre::Real(vp4->getActualHeight()));
		break;
	}
	}
}

void GameplayScene::ControllerInput()
{
	for (unsigned int i = 0; i < numLocalPlayers; ++i)
	{
		if (mControllers[i]->IsConnected())
		{
			if (mControllers[i]->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X)
			{
				UseItem(i);
			}
			
			//Left joystick for turning
			float lsx = mControllers[i]->GetState().Gamepad.sThumbLX;
			if (lsx < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || lsx > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			{
				mCars[i]->mTurning = lsx / 32767.0f;
			}
			else
			{
				mCars[i]->mTurning = 0.0f;
			}

			//Right trigger for acceleration
			float rt = mControllers[i]->GetState().Gamepad.bRightTrigger;
			if (rt >  XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				mCars[i]->mCanMoveForward = 1.0f;
			}
			else
			{
				mCars[i]->mCanMoveForward = 0.0f;
			}

			//Left trigger for reversing
			float lt = mControllers[i]->GetState().Gamepad.bLeftTrigger;
			if (lt > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				mCars[i]->mCanMoveBackward = 1.0f;
			}
			else
			{
				mCars[i]->mCanMoveBackward = 0.0f;
			}
		}
	}
}

void GameplayScene::UseItem(int carID)
{
	if (mCars[carID]->mCurrentItem == IBT_NONE)
		return;
	else if (mCars[carID]->mCurrentItem == IBT_ATTACK)
		FireMissile(carID);
	else if (mCars[carID]->mCurrentItem == IBT_DEFENCE)
		DropMine(carID);

	char cw[128];
	sprintf_s(cw, 128, "currentWeapon%i", carID);
	mSceneMgr->getEntity(cw)->setMaterialName("hud_empty");

	mCars[carID]->mCurrentItem = IBT_NONE;
}
void GameplayScene::FireMissile(int carID)
{
	std::shared_ptr<Missile> missile = std::shared_ptr<Missile>(new Missile("missile", mSceneMgr, mCars[carID]->GetSceneNode()));

	mPhysicsWorld->getWorld()->addRigidBody(missile->GetRigidBody());
	mPhysicsWorld->getWorld()->addCollisionObject(missile->ghost);

	btScalar mat[16];
	mCars[carID]->GetRigidBody()->getWorldTransform().getOpenGLMatrix(mat);
	btVector3 forward = btVector3(mat[8], mat[9], mat[10]);

	missile->GetRigidBody()->translate(forward * -50);

	forward *= -500;

	missile->setVelocity(forward.x(), forward.y(), forward.z());

	mActiveWeapons.push_back(missile);
}
void GameplayScene::DropMine(int carID)
{
	std::shared_ptr<Mine> mine = std::shared_ptr<Mine>(new Mine("mine", mSceneMgr, mCars[carID]->GetSceneNode()));

	mPhysicsWorld->getWorld()->addRigidBody(mine->GetRigidBody());
	mPhysicsWorld->getWorld()->addCollisionObject(mine->ghost);

	mActiveWeapons.push_back(mine);
}


//Menu Scene
MenuScene::MenuScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Client> client, std::shared_ptr<Ogre::Camera> cameras[4], std::shared_ptr<Ogre::RenderWindow> window, std::shared_ptr<Controller> controllers[4]) : Scene(sceneMgr, client, cameras, window, controllers)
{
	currentSubMenu = nextSubMenu = sm_Main;

	deltaT = 0.0f;

	SetUpViewports();
}
MenuScene::~MenuScene(){}

void MenuScene::KeyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_SPACE)
	{
		if (currentSubMenu == sm_Main)
		{
			nextSubMenu = sm_PlayerCount;
		}
		else if (currentSubMenu == sm_PlayerCount)
		{
			nextSubMenu = sm_LevelSelect;
		}
		else if (currentSubMenu == sm_LevelSelect)
		{
			SwapToGameplayLevel(mCurrentSelectedLevel);
		}
	}

	if (arg.key == OIS::KC_ESCAPE)
	{
		if (currentSubMenu == sm_Main)
		{
			exit(1);
		}
		else
		{
			nextSubMenu = sm_Main;
		}
	}

	if (arg.key == OIS::KC_1)
		numLocalPlayers = 1;
	if (arg.key == OIS::KC_2)
		numLocalPlayers = 2;
	if (arg.key == OIS::KC_3)
		numLocalPlayers = 3;
	if (arg.key == OIS::KC_4)
		numLocalPlayers = 4;
}
void MenuScene::KeyReleased(const OIS::KeyEvent &arg){}

void MenuScene::mouseMoved(const OIS::MouseEvent &arg){}
void MenuScene::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	Ogre::RaySceneQuery *q = GetSceneManager()->createRayQuery(Ogre::Ray());
	
	float x = arg.state.X.abs/float(mWindow->getWidth()), y = arg.state.Y.abs/float(mWindow->getHeight());

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
					nextSubMenu = sm_PlayerCount;
				}
				else if (itr->movable->getName() == "bExitEnt0")
				{
					//exit the game

					exit(1);
				}
			}
			else if (currentSubMenu == sm_PlayerCount)
			{
				Ogre::String str = itr->movable->getName();
				char buttonType[256];
				sscanf_s(str.c_str(), "%8s", &buttonType, 256);
				if (!stricmp(buttonType, "bPlayers"))
				{
					sscanf_s(str.c_str(),"%*[^_]_%i",&numLocalPlayers);

					nextSubMenu = sm_LevelSelect;
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
					GetSceneManager()->getSceneNode(mCurrentSelectedLevel + "MenuMini")->setPosition(0.5, 20, 50);

					sscanf_s(str.c_str(),"%*[^_]_%s",&levelName,256);
					mCurrentSelectedLevel = levelName;
					GetSceneManager()->getSceneNode(Ogre::String(levelName) + "MenuMini")->setPosition(0.5, 20, -5);
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
void MenuScene::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id){}

bool MenuScene::Update()
{
	timeStep += clock.getTimeSeconds();
	clock.reset();
	GetPhysicsWorld()->updateWorld(timeStep);

	ControllerInput();

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
	GetSceneManager()->getSceneNode(mCurrentSelectedLevel + "MenuMini")->setPosition(0.5, 20, -5);
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
void MenuScene::ControllerInput()
{

}

//Intro Scene
IntroScene::IntroScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Client> client, std::shared_ptr<Ogre::Camera> cameras[4], std::shared_ptr<Ogre::RenderWindow> window, std::shared_ptr<Controller> controllers[4]) : Scene(sceneMgr, client, cameras, window, controllers)
{
	SetUpViewports();
}

IntroScene::~IntroScene(){}

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