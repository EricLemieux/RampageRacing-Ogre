#include "Scene.h"

Scene::Scene(SceneArguments args)
{
	mSceneMgr = args.sceneMgr;

	mCamera = args.cameras[0];

	for (unsigned int i = 0; i < 4; ++i)
	{
		mCameras[i] = args.cameras[i];
		mControllers[i] = args.controllers[i];
	}

	ResetCamera();
	
	//Set up the physics world
	mPhysicsWorld = std::shared_ptr<PhysicsWorld>(new PhysicsWorld);
	mPhysicsWorld->initWorld();

	mWindow = args.window;

	mWindow->setActive(true);

	//mCarEnt = std::shared_ptr<Ogre::Entity>(mSceneMgr->createEntity("BoltCar", "BoltCar.mesh"));

	mGameClient = args.client;
	timeStep = 0;
	clock.reset();

	soundSys.initSound();
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

bool Scene::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
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
	newScene = std::shared_ptr<MenuScene>(new MenuScene(SceneArguments(mSceneMgr, this->mGameClient, mCameras, mWindow, mControllers, mSoundSys)));
	newScene->LoadLevel("MainMenu");
	swapToTheNewScene = true;
}

void Scene::SwapToGameplayLevel(Ogre::String levelName)
{
	GetSceneManager()->clearScene();
	lastSelected = "";
	newScene = std::shared_ptr<GameplayScene>(new GameplayScene(SceneArguments(mSceneMgr, this->mGameClient, mCameras, mWindow, mControllers, mSoundSys)));
	newScene->LoadLevel(levelName);
	newScene->AddCarToScene("myCar");
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
GameplayScene::GameplayScene(SceneArguments args) : Scene(args)
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

	AddTriggerVolumesToScene();
	SetUpItemBoxes();

	//Add the items so that they are already loaded into memory
	Ogre::Entity* dummyMissile = mSceneMgr->createEntity("Missile.mesh");
	Ogre::Entity* dummyMine = mSceneMgr->createEntity("Mine.mesh");
}

void GameplayScene::SetUpItemBoxes()
{
	srand(time(NULL));

	unsigned int count = mTriggerVolumes.size();
	for (unsigned int i = 1; i < count; ++i)
	{
		for (unsigned int j = 0; j < 3; ++j)
		{
			char name[128];
			sprintf_s(name, 128, "itemBox%i", (i*3)+j);

			Ogre::Vector3 pos;
			Ogre::Quaternion rot;

			char checkpointName[128];
			sprintf_s(checkpointName, 128, "checkpoint%i", i);
			Ogre::Matrix4 m = mSceneMgr->getSceneNode(checkpointName)->_getFullTransform();
			Ogre::Vector3 rightVector = Ogre::Vector3(m[0][0], m[1][0], m[2][0]).normalisedCopy();

			pos = mSceneMgr->getSceneNode(checkpointName)->getPosition();
			pos += rightVector * (30 - j * 30.0f);
			pos.y -= 5.0f;
			rot = mSceneMgr->getSceneNode(checkpointName)->getOrientation();

			ITEM_BOX_TYPE boxType = IBT_NONE;
			unsigned int val = rand() % 4;
			if (val == 0)
				boxType = IBT_ATTACK;
			else if (val == 1)
				boxType = IBT_DEFENCE;
			else if (val == 2)
				boxType = IBT_SPEED;
			else
				boxType = IBT_NONE;

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
		soundSys.playSound(CAR_ACCEL, P1);
	}
	else if (arg.key == OIS::KC_S)
	{
		mCar->mCanMoveBackward = 1.0f;
		soundSys.playSound(CAR_ACCEL, P1);
	}

	if (arg.key == OIS::KC_L)
	{
		//Attach a test particle to the car
		if (mSceneMgr->hasParticleSystem("Sparks"))
			mSceneMgr->destroyParticleSystem("Sparks");
		Ogre::ParticleSystem* particleSys = mSceneMgr->createParticleSystem("Sparks", "Sparks");
		if (mSceneMgr->hasSceneNode("Particle"))
			mSceneMgr->destroySceneNode("Particle");
		Ogre::SceneNode* particleNode = mCar->GetSceneNode()->createChildSceneNode("Particle");
		particleNode->attachObject(particleSys);
	}

	if (arg.key == OIS::KC_T)
	{
		char name[32];
		sprintf_s(name, 32, "checkpoint%i",mCar->lastCheckpoint);

		//The physics doesnt really like this but it works, so its fine for now
		btTransform trans(OgreToBtQuaternion(mSceneMgr->getSceneNode(name)->getOrientation()), OgreToBtVector3(mSceneMgr->getSceneNode(name)->getPosition()));
		mCar->GetRigidBody()->setWorldTransform(trans);
	}

	if (arg.key == OIS::KC_D)
	{
		mCar->mTurning = 1.0f;
	}
	if (arg.key == OIS::KC_A)
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

	if (arg.key == OIS::KC_M){
		soundSys.playSound(MEOW, BG);
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
		mCars[i] = std::shared_ptr<Car>(new Car(name, GetSceneManager(), GetPhysicsWorld()->getWorld(), selectedCarTypes[i], i, mCameras));
		mCarRankings[i] = mCars[i];
		
		callback = new ContactSensorCallback(*(mCars[i]->GetRigidBody()), *(mCars[i]));
	}

	mCar = mCars[0];	
}

void GameplayScene::AddTriggerVolumesToScene()
{
	//Create each checkpoint
	unsigned int count = 99;
	for (unsigned int i = 0; i < count; ++i)
	{
		char name[32];
		sprintf_s(name, 32, "checkpoint%i",i);

		if (mSceneMgr->hasSceneNode(name))
		{
			mTriggerVolumes.push_back(std::shared_ptr<TriggerVolume>(new TriggerVolume(name, GetSceneManager())));
			mPhysicsWorld->addBodyToWorld(mTriggerVolumes[i]->GetRigidBody());
		}
		else
			break;
	}
}

bool GameplayScene::Update()
{
	timeStep = clock.getTimeSeconds();
	int timeForCars = clock.getTimeMilliseconds();
	clock.reset();
	GetPhysicsWorld()->updateWorld(timeStep);

	ControllerInput();

	bool allDoneLooking = true;
	
	for (unsigned int i = 0; i < numLocalPlayers; ++i)
	{
		if (!mCars[i]->mFinishedRace)
			mCars[i]->raceTime += int(timeForCars);
		
		if (!mCars[i]->doneLookingAtResults)
			allDoneLooking = false;
		

		mCars[i]->Update();

		if (goingUp)
			bounce += 0.01f;
		else
			bounce -= 0.01f;
		if (bounce >= 1.0f || bounce <= -1.0f)
			goingUp = !goingUp;

		mCars[i]->GetSceneNode()->translate(0,bounce+2.0f,0);
		
		char shadowName[32];
		sprintf_s(shadowName, 32, "shadow%i", i);
		mSceneMgr->getSceneNode(shadowName)->setPosition(0, (-1.0f * (bounce*0.3f)) - 1.7f, 0);
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

			for (int k = 0; k < manifoldArray.size(); ++k){

					btPersistentManifold* manifold = manifoldArray[k];
					int numContacts = manifold->getNumContacts();
					for (int mi = 0; mi < numContacts; ++mi){
						const btManifoldPoint&pt = manifold->getContactPoint(mi);
						if (pt.getDistance() < 0.f)
						{
							const btVector3& ptA = pt.getPositionWorldOnA();
							const btVector3& ptB = pt.getPositionWorldOnB();
							const btVector3& normalOnB = pt.m_normalWorldOnB;
							/// work here
						
							unsigned int count = mTriggerVolumes.size();
							for (unsigned int tv = 0; tv < count; ++tv){
								if (tv != mCars[i]->lastCheckpoint && ((manifold->getBody0() == mTriggerVolumes[tv]->GetRigidBody() || manifold->getBody1() == mTriggerVolumes[tv]->GetRigidBody()) && (manifold->getBody0() == mCars[i]->ghost || manifold->getBody1() == mCars[i]->ghost))){
									mCars[i]->lastCheckpoint = tv;
									if (tv != 0)
									{
										mCars[i]->checkPointsHit++;
									}
									else if (mCars[i]->checkPointsHit > (count / 2) && tv == 0){
										mCars[i]->checkPointsHit = 0;
										mCars[i]->lapCounter++;

										if (mCars[i]->lapCounter > 2)
										{
											if (mNumPlayersCompletedRace == numLocalPlayers) //-1?
											{
												//TODO FIX
											}
											else
											{
												mCars[i]->mFinishedRace = true;
												mCars[i]->DisplayResults();
												mNumPlayersCompletedRace++;
											}
										}
										else
										{
											char m[128];
											sprintf_s(m, 128, "Lap %i/3", mCars[i]->lapCounter + 1);
											mCars[i]->lapText->setCaption(m);
										}
									}
									break;
								}
							}

							if (mCars[i]->lastCheckpoint != 0 && mCars[i]->lastCheckpoint != mCars[i]->lastItemBoxCheckpoint)
							{
								for (unsigned int ib = 0; ib < 3; ++ib)
								{
									unsigned int boxID = (mCars[i]->lastCheckpoint * 3) - 3 + ib;

									bool test0 = manifold->getBody0() == mItemBoxes[boxID]->GetRigidBody() ? true : false;
									bool test1 = manifold->getBody1() == mItemBoxes[boxID]->GetRigidBody() ? true : false;
									if ((test0 || test1) && mCars[i]->mCurrentItem == IBT_NONE)
									{
										mCars[i]->SetItem(mItemBoxes[boxID]->GetType());
										mCars[i]->lastItemBoxCheckpoint = mCars[i]->lastCheckpoint;
										break;
									}
								}
							}

							std::list<std::shared_ptr<GameObject>>::iterator itr = mActiveWeapons.begin();
							for (; itr != mActiveWeapons.end(); ++itr){
								if (manifold->getBody0() == (*itr)->GetRigidBody() || manifold->getBody1() == (*itr)->GetRigidBody()){
									btVector3 force = (*itr)->GetRigidBody()->getWorldTransform().getOrigin() - mCars[i]->GetRigidBody()->getWorldTransform().getOrigin();
									//HOTFIX FOR PRESENTATION
									//mCars[i]->GetRigidBody()->applyCentralImpulse(force * 1000);
									//if (i != (*itr)->ownerID){

									//}
									if ((*itr)->objectType == MISSILE && (*itr)->ownerID != mCars[i]->GetRigidBody()){
										//disable car controls
										mCars[i]->stunCounter = 100;
										break;
									}
									if ((*itr)->objectType == MINE && (*itr)->ownerID != mCars[i]->GetRigidBody()){
										//apply knockback
										mCars[i]->GetRigidBody()->setLinearVelocity(mCars[i]->GetRigidBody()->getLinearVelocity()*-0.5f);
										break;
									}
								}
							}

							if (mCars[i]->stunCounter == 100){
								if (mSceneMgr->hasParticleSystem("Sparks"))
									mSceneMgr->destroyParticleSystem("Sparks");
								Ogre::ParticleSystem* particleSys = mSceneMgr->createParticleSystem("Sparks", "Sparks");
								if (mSceneMgr->hasSceneNode("Particle"))
									mSceneMgr->destroySceneNode("Particle");
								Ogre::SceneNode* particleNode = mCars[i]->GetSceneNode()->createChildSceneNode("Particle");
								particleNode->attachObject(particleSys);
							}
						}
					}
			}

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
				bool test1 = manifold->getBody1() == obj->ownerID ? true : false;
				bool test0 = manifold->getBody0() == obj->ownerID ? true : false;
				bool test2 = false;
				bool test3 = false;
				for (int j = 0; j < numLocalPlayers; ++j){
					test2 = manifold->getBody0() == mCars[j]->GetRigidBody() ? true : false;
					test3 = manifold->getBody1() == mCars[j]->GetRigidBody() ? true : false;
				}

				if (((!test0 && !test1) && (test2 || test3)) || obj->lifeTimer > 500){
					//deactivate weapon
					btTransform transform = btTransform(btQuaternion(), btVector3(1000000000000000, 10000000000000, 100000000));
					obj->GetRigidBody()->setWorldTransform(transform);
					obj->GetRigidBody()->setDeactivationTime(10000000000000);
					obj->isActive = false;
				}
 					
			}
			//}
		}
	});

	for (unsigned int r = 0; r < numLocalPlayers; ++r)
	{
		unsigned int nextCheckpoint = mCarRankings[r]->lastCheckpoint + 1;
		if (nextCheckpoint >= mTriggerVolumes.size())
			nextCheckpoint = 0;
		if (r != 0)
		{
			//Compare ahead
			if ((mCarRankings[r]->lastCheckpoint > mCarRankings[r - 1]->lastCheckpoint && mCarRankings[r - 1]->lastCheckpoint != 0) ||
				((mCarRankings[r]->lastCheckpoint == mCarRankings[r - 1]->lastCheckpoint) && (mCarRankings[r]->GetSceneNode()->getPosition().squaredDistance(mTriggerVolumes[nextCheckpoint]->GetSceneNode()->getPosition()) < mCarRankings[r - 1]->GetSceneNode()->getPosition().squaredDistance(mTriggerVolumes[nextCheckpoint]->GetSceneNode()->getPosition()))))
			{
				//Swap the place
				std::shared_ptr<Car> temp = mCarRankings[r];
				mCarRankings[r] = mCarRankings[r - 1];
				mCarRankings[r - 1] = temp;
			}
		}
	}
	//Set the rankings for the screen
	for (unsigned int r = 0; r < numLocalPlayers; ++r)
	{
		char pos[8];
		sprintf_s(pos,8,"%i",r+1);
		mCarRankings[r]->positionText->setCaption(pos);
	}

	soundSys.update();

	if (allDoneLooking)
		SwapToMainMenu();

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
		vp1->setVisibilityMask(masks[0]);
	
		Ogre::Viewport* vp2 = mWindow->addViewport(mCameras[1].get(), 1);
		vp2->setDimensions(0, 0.5, 1, 0.5);
		mCameras[1]->setAspectRatio(Ogre::Real(vp2->getActualWidth()) / Ogre::Real(vp2->getActualHeight()));
		vp2->setVisibilityMask(masks[1]);
		break;
	}
	case 3:
	{
		Ogre::Viewport* vp1 = mWindow->addViewport(mCamera.get());
		vp1->setDimensions(0, 0, 0.5, 0.5);
		mCamera->setAspectRatio(Ogre::Real(vp1->getActualWidth()) / Ogre::Real(vp1->getActualHeight()));
		vp1->setVisibilityMask(masks[0]);
	
		Ogre::Viewport* vp2 = mWindow->addViewport(mCameras[1].get(), 1);
		vp2->setDimensions(0.5, 0, 0.5, 0.5);
		mCameras[1]->setAspectRatio(Ogre::Real(vp2->getActualWidth()) / Ogre::Real(vp2->getActualHeight()));
		vp2->setVisibilityMask(masks[1]);
	
		Ogre::Viewport* vp3 = mWindow->addViewport(mCameras[2].get(), 2);
		vp3->setDimensions(0, 0.5, 1, 0.5);
		mCameras[2]->setAspectRatio(Ogre::Real(vp3->getActualWidth()) / Ogre::Real(vp3->getActualHeight()));
		vp3->setVisibilityMask(masks[2]);
		break;
	}
	case 4:
	{
		Ogre::Viewport* vp1 = mWindow->addViewport(mCamera.get());
		vp1->setDimensions(0, 0, 0.5, 0.5);
		mCamera->setAspectRatio(Ogre::Real(vp1->getActualWidth()) / Ogre::Real(vp1->getActualHeight()));
		vp1->setVisibilityMask(masks[0]);
	
		Ogre::Viewport* vp2 = mWindow->addViewport(mCameras[1].get(), 1);
		vp2->setDimensions(0.5, 0, 0.5, 0.5);
		mCameras[1]->setAspectRatio(Ogre::Real(vp2->getActualWidth()) / Ogre::Real(vp2->getActualHeight()));
		vp2->setVisibilityMask(masks[1]);
	
		Ogre::Viewport* vp3 = mWindow->addViewport(mCameras[2].get(), 2);
		vp3->setDimensions(0, 0.5, 0.5, 0.5);
		mCameras[2]->setAspectRatio(Ogre::Real(vp3->getActualWidth()) / Ogre::Real(vp3->getActualHeight()));
		vp3->setVisibilityMask(masks[2]);
	
		Ogre::Viewport* vp4 = mWindow->addViewport(mCameras[3].get(), 3);
		vp4->setDimensions(0.5, 0.5, 0.5, 0.5);
		mCameras[3]->setAspectRatio(Ogre::Real(vp4->getActualWidth()) / Ogre::Real(vp4->getActualHeight()));
		vp4->setVisibilityMask(masks[3]);
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

			if (mControllers[i]->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
			{
				char name[32];
				sprintf_s(name, 32, "checkpoint%i", mCars[i]->lastCheckpoint);

				//The physics doesnt really like this but it works, so its fine for now
				btTransform trans(OgreToBtQuaternion(mSceneMgr->getSceneNode(name)->getOrientation()), OgreToBtVector3(mSceneMgr->getSceneNode(name)->getPosition()));
				mCars[i]->GetRigidBody()->setWorldTransform(trans);

				mCars[i]->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
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

			if (mControllers[i]->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START)
			{
				if (mCars[i]->mFinishedRace == true)
				{
					mCars[i]->doneLookingAtResults = true;
				}
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

	mCars[carID]->SetItem(IBT_NONE);
}
void GameplayScene::FireMissile(int carID)
{
	std::shared_ptr<Missile> missile = std::shared_ptr<Missile>(new Missile("missile", mSceneMgr, mCars[carID]->GetSceneNode(), mCars[carID]->GetRigidBody()));

	mPhysicsWorld->getWorld()->addRigidBody(missile->GetRigidBody());
	mPhysicsWorld->getWorld()->addCollisionObject(missile->ghost);

	btScalar mat[16];
	mCars[carID]->GetRigidBody()->getWorldTransform().getOpenGLMatrix(mat);
	btVector3 forward = btVector3(mat[8], mat[9], mat[10]);

	missile->GetRigidBody()->translate(forward * -50);

	forward *= -5000;

	missile->setVelocity(forward.x()/5, forward.y()/5, forward.z()/5);
	missile->GetRigidBody()->applyCentralForce(btVector3(forward.x(), forward.y(), forward.z()));

	mActiveWeapons.push_back(missile);
}
void GameplayScene::DropMine(int carID)
{
	std::shared_ptr<Mine> mine = std::shared_ptr<Mine>(new Mine("mine", mSceneMgr, mCars[carID]->GetSceneNode(), mCars[carID]->GetRigidBody()));

	mPhysicsWorld->getWorld()->addRigidBody(mine->GetRigidBody());
	mPhysicsWorld->getWorld()->addCollisionObject(mine->ghost);

	mActiveWeapons.push_back(mine);
}


//Menu Scene
MenuScene::MenuScene(SceneArguments args) : Scene(args)
{
	currentSubMenu = nextSubMenu = sm_Main;

	deltaT = 0.0f;

	SetUpViewports();

	//Give the lobby a title so the players know to ready up
	Ogre::SceneNode* n = mSceneMgr->getRootSceneNode()->createChildSceneNode("lobbyTitle");
	Ogre::MovableText* t = new Ogre::MovableText("lobbyTitle", "Press Start to Ready Up");
	t->setTextAlignment(Ogre::MovableText::H_LEFT, Ogre::MovableText::V_CENTER);
	n->setPosition(26.2, 21.8, -5);
	n->scale(0.25, 0.25, 1);
	n->attachObject(t);

	//Set up the mesh names
	carMeshes[0] = "BoltCar.mesh";
	carMeshes[1] = "OnyxCar.mesh";
	carMeshes[2] = "ViperCar.mesh";
	carMeshes[3] = "NeonCar.mesh";
	carMeshes[4] = "CitoCar.mesh";

	carSelectionTitleNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("carSelectionTitleNode");
	carSelectionTitle = new Ogre::MovableText("carSelectionTitle", "Player 1 Select");
	carSelectionTitle->setTextAlignment(Ogre::MovableText::H_LEFT, Ogre::MovableText::V_CENTER);
	carSelectionTitleNode->setPosition(-32.3, 21.6, -5);
	carSelectionTitleNode->scale(1, 0.5, 1);
	carSelectionTitleNode->attachObject(carSelectionTitle);
}
MenuScene::~MenuScene(){}

void MenuScene::KeyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_SPACE)
	{
		if (currentSubMenu == sm_Lobby)
		{
			SwapToGameplayLevel(mCurrentSelectedLevel);
			
		}
		else if (currentSubMenu == sm_CarSelect)
		{
			ConfirmCarChoice();
		}
		else
		{
			nextSubMenu = static_cast<subMenus>(currentSubMenu + 1);
		}
		soundSys.playSound(B_SELECT, BG);
	}

	if (arg.key == OIS::KC_ESCAPE)
	{
		if (currentSubMenu == sm_Main)
		{
			exit(1);
		}
		else
		{
			playerSelectingCar = 0;
			nextSubMenu = sm_Main;
		}
		soundSys.playSound(B_RETURN, BG);
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

void MenuScene::mouseMoved(const OIS::MouseEvent &arg)
{
	Ogre::RaySceneQuery *q = GetSceneManager()->createRayQuery(Ogre::Ray());

	float x = arg.state.X.abs / float(mWindow->getWidth()), y = arg.state.Y.abs / float(mWindow->getHeight());

	Ogre::Ray ray = GetCamera()->getCameraToViewportRay(x, y);

	q->setRay(ray);
	q->setSortByDistance(true);

	Ogre::RaySceneQueryResult res = q->execute();
	Ogre::RaySceneQueryResult::iterator itr = res.begin();

	if (lastSelected != "")
	{
		mSceneMgr->getEntity(lastSelected)->setMaterialName("button");
	}

	//Loop to select the object
	for (itr; itr != res.end(); itr++)
	{
		if (itr->movable && itr->movable->getName()[0] == 'b')
		{
			mSceneMgr->getEntity(itr->movable->getName())->setMaterialName("button_highlighted");
			lastSelected = itr->movable->getName();
		}
	}
}
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
			SelectButton(itr->movable->getName());
			if (currentSubMenu == sm_Lobby)
			{
				if (itr->movable->getName() == "bForceStart")
				{
					if (mCurrentSelectedLevel != "")
					{
						SwapToGameplayLevel(mCurrentSelectedLevel);
					}
				}
			}
			soundSys.playSound(B_SELECT, BG);
			break;
		}
	}
}
void MenuScene::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id){}

void MenuScene::SetUpLabelsAndCars()
{
	labels.clear();

	//Set up the lobby with the correct number of players
	for (unsigned int i = 0; i < numLocalPlayers; ++i)
	{
		//make the node
		char name[128];
		sprintf_s(name, 128, "playerLabel%i", i);

		if (mSceneMgr->hasSceneNode(name))
			mSceneMgr->destroySceneNode(name);

		Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);

		//Make the checkbox ent
		Ogre::Entity* ent = mSceneMgr->createEntity("checkBox.mesh");

		//Make the text
		char words[128];
		sprintf_s(words, 128, "Player %i", i + 1);

		Ogre::MovableText* text = new Ogre::MovableText(name, words);
		text->setTextAlignment(Ogre::MovableText::H_LEFT, Ogre::MovableText::V_CENTER);
		node->setPosition(26.5, 21.3 - (i*0.6), -5);
		node->scale(1, 0.5, 1);

		//Combine it
		std::shared_ptr<PlayerLabel> p = std::shared_ptr<PlayerLabel>(new PlayerLabel(node, ent, text));

		labels.push_back(p);
	}
}

Ogre::Entity* MenuScene::GetNextCarModel(int difference)
{
	if ((carIndex + difference) > 4)
		carIndex = -1;
	else if ((carIndex + difference) < 0)
		carIndex = 4;
	carIndex += difference;

	Ogre::Entity* ent = mSceneMgr->createEntity("CarSelector", carMeshes[carIndex]);

	selectedCarTypes[playerSelectingCar] = carMeshes[carIndex];

	return ent;
}

void MenuScene::ConfirmCarChoice()
{
	playerSelectingCar++;

	char words[128];
	sprintf_s(words, 128, "Player %i Select", playerSelectingCar+1);
	carSelectionTitle->setCaption(words);

	if (playerSelectingCar >= numLocalPlayers)
		nextSubMenu = static_cast<subMenus>(currentSubMenu + 1);
}

bool MenuScene::Update()
{
	timeStep += clock.getTimeSeconds();
	clock.reset();
	GetPhysicsWorld()->updateWorld(timeStep);

	mSceneMgr->getSceneNode("CarSelector")->rotate(Ogre::Vector3(0.0f, 1.0f, 0.0f), Ogre::Radian(0.001f));

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

	ControllerInput();

	return true;
}

bool MenuScene::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	if (!this->swapToTheNewScene)
		animationState->addTime(evt.timeSinceLastFrame);

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

	Ogre::Animation::setDefaultInterpolationMode(Ogre::Animation::IM_LINEAR);
	Ogre::Animation::setDefaultRotationInterpolationMode(Ogre::Animation::RIM_LINEAR);

	mSceneMgr->getEntity("Robot")->getAllAnimationStates()->createAnimationState("Idle", 0, 1000);
	animationState = mSceneMgr->getEntity("Robot")->getAnimationState("Idle");
	animationState->setLoop(true);
	animationState->setEnabled(true);
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
	for (unsigned int i = 0; i < numLocalPlayers; ++i)
	{
		if (lastSelected != "")
			mSceneMgr->getEntity(lastSelected)->setMaterialName("button");
		if (mControllers[i]->IsConnected())
		{
			if (currentSubMenu == sm_Main)
			{
				float lsy = mControllers[i]->GetState().Gamepad.sThumbLY;
				if (lsy < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					lastSelected = "bExit";
					soundSys.playSound(B_SELECT, BG);
				}
				else if (lsy > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					lastSelected = "bStart";
					soundSys.playSound(B_SELECT, BG);
				}
			}
			else if (currentSubMenu == sm_PlayerCount)
			{
				float lsy = mControllers[i]->GetState().Gamepad.sThumbLY;
				if (lsy < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					if (lastSelected == "bPlayers_1")
						lastSelected = "bPlayers_3";
					else if (lastSelected == "bPlayers_2")
						lastSelected = "bPlayers_4";
					soundSys.playSound(B_SELECT, BG);
				}
				else if (lsy > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					if (lastSelected == "bPlayers_3")
						lastSelected = "bPlayers_1";
					else if (lastSelected == "bPlayers_4")
						lastSelected = "bPlayers_2";
					soundSys.playSound(B_SELECT, BG);
				}

				float lsx = mControllers[i]->GetState().Gamepad.sThumbLX;
				if (lsx < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					if (lastSelected == "bPlayers_2")
						lastSelected = "bPlayers_1";
					else if (lastSelected == "bPlayers_4")
						lastSelected = "bPlayers_3";
					soundSys.playSound(B_SELECT, BG);
				}
				else if (lsx > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					if (lastSelected == "bPlayers_1")
						lastSelected = "bPlayers_2";
					else if (lastSelected == "bPlayers_3")
						lastSelected = "bPlayers_4";
					soundSys.playSound(B_SELECT, BG);
				}
			}
			else if (currentSubMenu == sm_LevelSelect)
			{
				//TODO
			}
			else if (currentSubMenu == sm_CarSelect)
			{
				float lsx = mControllers[i]->GetState().Gamepad.sThumbLX;
				if (lsx < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					if (lThumbWaited)
					{
						if (lastSelected == "bNextCar")
							lastSelected = "bSelect";
						else if (lastSelected == "bSelect")
							lastSelected = "bPrevCar";
						soundSys.playSound(B_SELECT, BG);
					}
					lThumbWaited = false;
				}
				else if (lsx > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					if (lThumbWaited)
					{
						if (lastSelected == "bPrevCar")
							lastSelected = "bSelect";
						else if (lastSelected == "bSelect")
							lastSelected = "bNextCar";
						soundSys.playSound(B_SELECT, BG);
					}
					lThumbWaited = false;
				}
				else
				{
					lThumbWaited = true;
				}
			}
			else if (currentSubMenu == sm_Lobby)
			{
				if ((mControllers[i]->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START) || (mControllers[i]->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A))
				{
					if (buttonAWaited)
					{
						labels[i]->SetReadyToPlay(!labels[i]->GetReadyToPlay());

						bool allReady = true;

						//loop through all of the other labels to see if they are ready to play
						for (unsigned int j = 0; j < labels.size(); ++j)
						{
							if (!labels[j]->GetReadyToPlay())
								allReady = false;
						}


						if (allReady)
						{
							soundSys.playSound(B_RETURN, BG);
							SwapToGameplayLevel(mCurrentSelectedLevel);
						}
						buttonAWaited = false;
					}					
				}
				else
				{
					buttonAWaited = true;
				}
			}

			if (mControllers[i]->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
			{
				if (buttonAWaited)
					SelectButton(lastSelected);
				buttonAWaited = false;
				soundSys.playSound(B_RETURN, BG);
			}
			else
			{
				buttonAWaited = true;
			}
			if (mControllers[i]->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)
			{
				nextSubMenu = sm_Main;
				lastSelected = "bStart";
				soundSys.playSound(B_RETURN, BG);
			}
		}
		if (lastSelected != "")
			mSceneMgr->getEntity(lastSelected)->setMaterialName("button_highlighted");
	}
}

void MenuScene::SelectButton(Ogre::String bName)
{
	if (currentSubMenu == sm_Main)
	{
		if (bName == "bStart")
		{
			nextSubMenu = static_cast<subMenus>(currentSubMenu + 1);
			lastSelected = "bPlayers_1";
		}
		else if (bName == "bExit")
		{
			//exit the game
			exit(1);
		}
	}
	else if (currentSubMenu == sm_PlayerCount)
	{
		char buttonType[256];
		sscanf_s(bName.c_str(), "%8s", &buttonType, 256);
		if (!stricmp(buttonType, "bPlayers"))
		{
			sscanf_s(bName.c_str(), "%*[^_]_%i", &numLocalPlayers);

			SetUpLabelsAndCars();

			nextSubMenu = static_cast<subMenus>(currentSubMenu + 1);
			lastSelected = "bPlay";
		}
	}
	else if (currentSubMenu == sm_LevelSelect)
	{
		char buttonType[256], levelName[256];
		sscanf_s(bName.c_str(), "%6s", &buttonType, 256);
		if (!stricmp(buttonType, "bLevel"))
		{
			//Move the current Selection back
			GetSceneManager()->getSceneNode(mCurrentSelectedLevel + "MenuMini")->setPosition(0.5, 20, 50);

			sscanf_s(bName.c_str(), "%*[^_]_%s", &levelName, 256);
			mCurrentSelectedLevel = levelName;
			GetSceneManager()->getSceneNode(Ogre::String(levelName) + "MenuMini")->setPosition(0.5, 20, -5);
		}

		else if (bName == "bPlay")
		{
			if (mCurrentSelectedLevel != "")
			{
				nextSubMenu = static_cast<subMenus>(currentSubMenu + 1);
				lastSelected = "bSelect";
			}
		}

		else if (bName == "bHome")
		{
			nextSubMenu = sm_Main;
		}
	}
	else if (currentSubMenu == sm_CarSelect)
	{
		if (!stricmp(bName.c_str(), "bNextCar"))
		{
			//Remove the current Child
			mSceneMgr->getSceneNode("CarSelector")->removeAndDestroyAllChildren();
			mSceneMgr->destroyEntity("CarSelector");

			//attach the new child
			Ogre::Entity* selectorEnt = GetNextCarModel(1);
			mSceneMgr->getSceneNode("CarSelector")->attachObject(selectorEnt);
		}
		else if (!stricmp(bName.c_str(), "bPrevCar"))
		{
			//Remove the current Child
			mSceneMgr->getSceneNode("CarSelector")->removeAndDestroyAllChildren();
			mSceneMgr->destroyEntity("CarSelector");

			//attach the new child
			Ogre::Entity* selectorEnt = GetNextCarModel(-1);
			mSceneMgr->getSceneNode("CarSelector")->attachObject(selectorEnt);
		}
		else if (!stricmp(bName.c_str(), "bSelect"))
		{
			ConfirmCarChoice();
		}
	}
	else if (currentSubMenu == sm_Lobby)
	{

	}
}

//Intro Scene
IntroScene::IntroScene(SceneArguments args) : Scene(args)
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