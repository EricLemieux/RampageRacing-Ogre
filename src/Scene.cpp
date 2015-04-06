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

	mSoundSys = args.soundSys;
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
		mCameras[i]->lookAt(0.0f, 1.0f, 0.0f);
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
	mSoundSys->playSound(BGM, 31);
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
		mSoundSys->playSound(CAR_ACCEL, 0);
	}
	else if (arg.key == OIS::KC_S)
	{
		mCar->mCanMoveBackward = 1.0f;
		mSoundSys->playSound(CAR_ACCEL, 0);
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
		mSoundSys->pauseSound(31);
		SwapToMainMenu();
	}

	if (arg.key == OIS::KC_M){
		mSoundSys->playSound(MEOW, 30);
	}
}
void GameplayScene::KeyReleased(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_W)
	{
		mCar->mCanMoveForward = 0.0f;
		mSoundSys->pauseSound(0);
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
	mCarRankings = new std::shared_ptr<Car>[numLocalPlayers];

	for (unsigned int i = 0; i < numTotalPlayers; ++i)
	{
		char name[128];
		sprintf_s(name, 128, "mCar%i",i);

		//Create a game object thing
		auto car = std::shared_ptr<Car>(new Car(name, GetSceneManager(), GetPhysicsWorld()->getWorld(), selectedCarTypes[i], i));
		
		callback = new ContactSensorCallback(*(car->GetRigidBody()), *(car));

		mCars.push_back(car);
	}

	for (unsigned int i = 0; i < numLocalPlayers; ++i)
	{
		mLocalCars[i] = mCars[mGameClient->startingIndex + i];
		mLocalCars[i]->isLocal = true;
		mLocalCars[i]->SetUpLocal(mCameras[i]);
		mCarRankings[i] = mLocalCars[i];
	}

	mCar = mLocalCars[0];	
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
	//Get the positions from the server for the other cars
	mGameClient->Recieve();

	if (!mGameClient->allDoneLoading)
	{
		if (!toldServerReady)
		{
			//Tell the server that we got this far and now waiting for everyone else
			char buffer[32];
			sprintf_s(buffer, "doneLoading %d", numLocalPlayers);
			mGameClient->SendString(buffer);

			toldServerReady = true;
		}

		return true;
	}

	//mSoundSys->playSound(BGM, BM);
	timeStep = clock.getTimeSeconds();
	int timeForCars = clock.getTimeMilliseconds();
	timeBetweenNetworkSend += clock.getTimeSeconds();
	clock.reset();
	GetPhysicsWorld()->updateWorld(timeStep);

	ControllerInput();

	bool allDoneLooking = true;
	
	for (unsigned int i = 0; i < numTotalPlayers; ++i)
	{
		if (!mCars[i]->mFinishedRace)
			mCars[i]->raceTime += int(timeForCars);
		
		if (!mCars[i]->doneLookingAtResults)
			allDoneLooking = false;
		
		if (mCars[i]->isLocal)
		{
			mCars[i]->Update();

			if (timeBetweenNetworkSend >= EXPECTED_TIME_BETWEEN_NETWORK_UPDATES)
			{
				//Send the position of the players car to the server
				{
					char str[256];
					auto pos = mCars[i]->GetSceneNode()->getPosition();
					sprintf_s(str, 256, "pos %d %f %f %f", i, pos.x, pos.y, pos.z);
					mGameClient->SendString(std::string(str));
				}

				//Send the rotation of the car to the server
				{
					char str[256];
					auto rot = mCars[i]->GetSceneNode()->getOrientation();
					sprintf_s(str, 256, "rot %d %f %f %f %f", i, rot.x, rot.y, rot.z, rot.w);
					mGameClient->SendString(std::string(str));
				}
				
				timeBetweenNetworkSend = 0.0f;
			}
		}
		else
		{
			mGameClient->Update( timeStep, i);
			mCars[i]->SetPosition(mGameClient->GetPos(i));
			mCars[i]->SetRotation(mGameClient->GetRot(i));
		}
		

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

	//COLLISION DETECTION
	btDiscreteDynamicsWorld* world = GetPhysicsWorld()->getWorld();

	for (int i = 0; i < numLocalPlayers; ++i){
		world->getDispatcher()->dispatchAllCollisionPairs(mLocalCars[i]->ghost->getOverlappingPairCache(), world->getDispatchInfo(), world->getDispatcher());
		//btTransform transform = mCar->ghost->getWorldTransform();
		btManifoldArray manifoldArray;

		for (int j = 0; j < mLocalCars[i]->ghost->getOverlappingPairCache()->getNumOverlappingPairs(); ++j){
			manifoldArray.resize(0);
			btBroadphasePair* collisionPair = &(mLocalCars[i]->ghost->getOverlappingPairCache()->getOverlappingPairArray()[j]);
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
								if (tv != mLocalCars[i]->lastCheckpoint && ((manifold->getBody0() == mTriggerVolumes[tv]->GetRigidBody() || manifold->getBody1() == mTriggerVolumes[tv]->GetRigidBody()) && (manifold->getBody0() == mLocalCars[i]->ghost || manifold->getBody1() == mLocalCars[i]->ghost))){
									mLocalCars[i]->lastCheckpoint = tv;
									if (tv != 0)
									{
										mLocalCars[i]->checkPointsHit++;
									}
									else if (mLocalCars[i]->checkPointsHit > (count / 2) && tv == 0){
										mLocalCars[i]->checkPointsHit = 0;
										mLocalCars[i]->lapCounter++;

										if (mLocalCars[i]->lapCounter > 2)
										{
											if (mNumPlayersCompletedRace == numLocalPlayers) //-1?
											{
												//TODO FIX
											}
											else
											{
												mLocalCars[i]->mFinishedRace = true;
												char* res = mLocalCars[i]->DisplayResults();
												mGameClient->SendString(res);
												mNumPlayersCompletedRace++;
											}
										}
										else
										{
											char m[128];
											sprintf_s(m, 128, "Lap %i/3", mLocalCars[i]->lapCounter + 1);
											mLocalCars[i]->lapText->setCaption(m);
										}
									}
									break;
								}
							}

							if (mLocalCars[i]->lastCheckpoint != 0 && mLocalCars[i]->lastCheckpoint != mLocalCars[i]->lastItemBoxCheckpoint)
							{
								for (unsigned int ib = 0; ib < 3; ++ib)
								{
									unsigned int boxID = (mLocalCars[i]->lastCheckpoint * 3) - 3 + ib;

									bool test0 = manifold->getBody0() == mItemBoxes[boxID]->GetRigidBody() ? true : false;
									bool test1 = manifold->getBody1() == mItemBoxes[boxID]->GetRigidBody() ? true : false;
									if ((test0 || test1) && mLocalCars[i]->mCurrentItem == IBT_NONE)
									{
										mLocalCars[i]->SetItem(mItemBoxes[boxID]->GetType());
										mLocalCars[i]->lastItemBoxCheckpoint = mLocalCars[i]->lastCheckpoint;
										mSoundSys->playSound(ITEM_PICKUP, (mLocalCars[i]->mID * 2));
										break;
									}
								}
							}

							bool carImpacted = false;
							std::list<std::shared_ptr<GameObject>>::iterator itr = mActiveWeapons.begin();
							for (; itr != mActiveWeapons.end(); ++itr){
								if (manifold->getBody0() == (*itr)->GetRigidBody() || manifold->getBody1() == (*itr)->GetRigidBody()){
									btVector3 force = (*itr)->GetRigidBody()->getWorldTransform().getOrigin() - mLocalCars[i]->GetRigidBody()->getWorldTransform().getOrigin();
									//HOTFIX FOR PRESENTATION
									//mLocalCars[i]->GetRigidBody()->applyCentralImpulse(force * 1000);
									//if (i != (*itr)->ownerID){

									//}
									if ((*itr)->objectType == MISSILE && (*itr)->ownerID != mLocalCars[i]->GetRigidBody()){
										//disable car controls
										mLocalCars[i]->stunCounter = 100;
										carImpacted = true;
										break;
									}
									if ((*itr)->objectType == MINE && (*itr)->ownerID != mLocalCars[i]->GetRigidBody()){
										//apply knockback
										mLocalCars[i]->GetRigidBody()->setLinearVelocity(mLocalCars[i]->GetRigidBody()->getLinearVelocity()*-0.5f);
										carImpacted = true;
										break;
									}
								}
							}

							if (carImpacted){
								if (mSceneMgr->hasParticleSystem("Sparks"))
									mSceneMgr->destroyParticleSystem("Sparks");
								Ogre::ParticleSystem* particleSys = mSceneMgr->createParticleSystem("Sparks", "Sparks");
								if (mSceneMgr->hasSceneNode("Particle"))
									mSceneMgr->destroySceneNode("Particle");
								Ogre::SceneNode* particleNode = mLocalCars[i]->GetSceneNode()->createChildSceneNode("Particle");
								particleNode->attachObject(particleSys);
								mSoundSys->playSound(ITEM_EXPLODE, (mLocalCars[i]->mID * 2));
							}
						}
					}
			}

		}
		if (mLocalCars[i]->isAccelerating > 0){
			mSoundSys->playSound(CAR_STEADY, (mLocalCars[i]->mID*2) + 1);
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

			for (int j = 0; j < manifoldArray.size(); ++j){

				btPersistentManifold* manifold = manifoldArray[j];
				int numContacts = manifold->getNumContacts();
				for (int mi = 0; mi < numContacts; ++mi){
					const btManifoldPoint&pt = manifold->getContactPoint(mi);
					if (pt.getDistance() < 0.f)
					{
						const btVector3& ptA = pt.getPositionWorldOnA();
						const btVector3& ptB = pt.getPositionWorldOnB();
						const btVector3& normalOnB = pt.m_normalWorldOnB;
						/// work here
						bool test1 = manifold->getBody1() == obj->ownerID ? true : false;
						bool test0 = manifold->getBody0() == obj->ownerID ? true : false;
						bool test2 = false;
						bool test3 = false;
						for (int j = 0; j < numTotalPlayers; ++j){
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
				}
			}
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

	mSoundSys->update();

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

		Ogre::CompositorManager::getSingleton().addCompositor(vp, "Bloom");
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Bloom", true);

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
				sprintf_s(name, 32, "checkpoint%i", mLocalCars[i]->lastCheckpoint);

				//The physics doesnt really like this but it works, so its fine for now
				btTransform trans(OgreToBtQuaternion(mSceneMgr->getSceneNode(name)->getOrientation()), OgreToBtVector3(mSceneMgr->getSceneNode(name)->getPosition()));
				mLocalCars[i]->GetRigidBody()->setWorldTransform(trans);

				mLocalCars[i]->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
			}
			
			//Left joystick for turning
			float lsx = mControllers[i]->GetState().Gamepad.sThumbLX;
			if (lsx < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || lsx > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			{
				mLocalCars[i]->mTurning = lsx / 32767.0f;
			}
			else
			{
				mLocalCars[i]->mTurning = 0.0f;
			}

			//Right trigger for acceleration
			float rt = mControllers[i]->GetState().Gamepad.bRightTrigger;
			if (rt >  XINPUT_GAMEPAD_TRIGGER_THRESHOLD || mControllers[i]->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
			{
				mLocalCars[i]->mCanMoveForward = 1.0f;
				if (mLocalCars[i]->isAccelerating == 0)
					mSoundSys->playSound(CAR_ACCEL, (mLocalCars[i]->mID * 2) + 1);
				mLocalCars[i]->isAccelerating++;
			}
			else
			{
				mLocalCars[i]->mCanMoveForward = 0.0f;
				mSoundSys->pauseSound((mLocalCars[i]->mID * 2) + 1);
				mLocalCars[i]->isAccelerating = 0;
			}

			//Left trigger for reversing
			float lt = mControllers[i]->GetState().Gamepad.bLeftTrigger;
			if (lt > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				mLocalCars[i]->mCanMoveBackward = 1.0f;
			}
			else
			{
				mLocalCars[i]->mCanMoveBackward = 0.0f;
			}

			if (mControllers[i]->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START)
			{
				if (mLocalCars[i]->mFinishedRace == true)
				{
					mLocalCars[i]->doneLookingAtResults = true;
				}
			}
		}
	}
}

void GameplayScene::UseItem(int carID)
{
	if (mLocalCars[carID]->mCurrentItem == IBT_NONE)
		return;
	else if (mLocalCars[carID]->mCurrentItem == IBT_ATTACK)
		FireMissile(carID);
	else if (mLocalCars[carID]->mCurrentItem == IBT_DEFENCE)
		DropMine(carID);

	mLocalCars[carID]->SetItem(IBT_NONE);
}
void GameplayScene::FireMissile(int carID)
{
	std::shared_ptr<Missile> missile = std::shared_ptr<Missile>(new Missile("missile", mSceneMgr, mLocalCars[carID]->GetSceneNode(), mLocalCars[carID]->GetRigidBody()));

	mPhysicsWorld->getWorld()->addRigidBody(missile->GetRigidBody());
	mPhysicsWorld->getWorld()->addCollisionObject(missile->ghost);

	btScalar mat[16];
	mLocalCars[carID]->GetRigidBody()->getWorldTransform().getOpenGLMatrix(mat);
	btVector3 forward = btVector3(mat[8], mat[9], mat[10]);

	missile->GetRigidBody()->translate(forward * -50);

	forward *= -5000;

	missile->setVelocity(forward.x()/5, forward.y()/5, forward.z()/5);
	missile->GetRigidBody()->applyCentralForce(btVector3(forward.x(), forward.y(), forward.z()));

	mActiveWeapons.push_back(missile);
}
void GameplayScene::DropMine(int carID)
{
	std::shared_ptr<Mine> mine = std::shared_ptr<Mine>(new Mine("mine", mSceneMgr, mLocalCars[carID]->GetSceneNode(), mLocalCars[carID]->GetRigidBody()));

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
MenuScene::~MenuScene()
{
	mGameClient->allDoneLoading = false;
}

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
		mSoundSys->playSound(B_SELECT, 30);
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
		mSoundSys->playSound(B_RETURN, 30);
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
						for (unsigned int players = 0; players < numLocalPlayers; ++players)
						{
							if (!labels[players]->GetReadyToPlay())
							{
								mGameClient->SendString("ready");
							}

							labels[players]->SetReadyToPlay(true);
						}
					}
				}
			}
			
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
		sprintf_s(words, 128, "Player %i", i + 1 + mGameClient->startingIndex);

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
	timeBetweenControllerInput += 1;

	mGameClient->Recieve();

	if (wantsToSetUpLabels && mGameClient->startingIndex != 999)
	{
		SetUpLabelsAndCars();
		wantsToSetUpLabels = false;
	}

	//Set the total number of connected players
	numTotalPlayers = mGameClient->totalPlayers;

	if (mGameClient->allReady)
	{
		mSoundSys->playSound(B_RETURN, 30);
		SwapToGameplayLevel(mCurrentSelectedLevel);
	}

	timeStep += clock.getTimeSeconds();
	clock.reset();
	GetPhysicsWorld()->updateWorld(timeStep);

	if (mSceneMgr->hasSceneNode("CarSelector"))
		mSceneMgr->getSceneNode("CarSelector")->rotate(Ogre::Vector3(0.0f, 1.0f, 0.0f), Ogre::Radian(0.001f));

	if (currentSubMenu == subMenus::sm_LevelSelect && mCurrentSelectedLevel != "")
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

	//Add the robot to the scene
	robotNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Robot");
	robotEnt = mSceneMgr->createEntity("Robot", "AnimatedNPC.mesh");
	robotNode->setScale(0.7, 0.7, 0.7);
	robotNode->setPosition(5, 0, -20);
	robotNode->attachObject(robotEnt);

	Ogre::Animation::setDefaultInterpolationMode(Ogre::Animation::IM_LINEAR);
	Ogre::Animation::setDefaultRotationInterpolationMode(Ogre::Animation::RIM_LINEAR);
	
	animationState = robotEnt->getAnimationState("idle");
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
	if (timeBetweenControllerInput <= 50)
		return;
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
					mSoundSys->playSound(B_SELECT, 30);
					timeBetweenControllerInput = 0;
				}
				else if (lsy > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					lastSelected = "bStart";
					mSoundSys->playSound(B_SELECT, 30);
					timeBetweenControllerInput = 0;
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
					mSoundSys->playSound(B_SELECT, 30);
					timeBetweenControllerInput = 0;
				}
				else if (lsy > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					if (lastSelected == "bPlayers_3")
						lastSelected = "bPlayers_1";
					else if (lastSelected == "bPlayers_4")
						lastSelected = "bPlayers_2";
					mSoundSys->playSound(B_SELECT, 30);
					timeBetweenControllerInput = 0;
				}

				float lsx = mControllers[i]->GetState().Gamepad.sThumbLX;
				if (lsx < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					if (lastSelected == "bPlayers_2")
						lastSelected = "bPlayers_1";
					else if (lastSelected == "bPlayers_4")
						lastSelected = "bPlayers_3";
					mSoundSys->playSound(B_SELECT, 30);
					timeBetweenControllerInput = 0;
				}
				else if (lsx > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					if (lastSelected == "bPlayers_1")
						lastSelected = "bPlayers_2";
					else if (lastSelected == "bPlayers_3")
						lastSelected = "bPlayers_4";
					mSoundSys->playSound(B_SELECT, 30);
					timeBetweenControllerInput = 0;
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
					if (lastSelected == "bNextCar")
						lastSelected = "bSelect";
					else if (lastSelected == "bSelect")
						lastSelected = "bPrevCar";
					mSoundSys->playSound(B_SELECT, 30);
					timeBetweenControllerInput = 0;
				}
				else if (lsx > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					if (lastSelected == "bPrevCar")
						lastSelected = "bSelect";
					else if (lastSelected == "bSelect")
						lastSelected = "bNextCar";
					mSoundSys->playSound(B_SELECT, 30);
					timeBetweenControllerInput = 0;
				}
			}
			else if (currentSubMenu == sm_Lobby)
			{
				if ((mControllers[i]->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START) || (mControllers[i]->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A))
				{
					timeBetweenControllerInput = 0;
					labels[i]->SetReadyToPlay(!labels[i]->GetReadyToPlay());

					if (labels[i]->GetReadyToPlay())
					{
						mGameClient->SendString("ready");
					}
					else
					{
						mGameClient->SendString("notready");
					}		
				}
			}

			if (mControllers[i]->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
			{
				timeBetweenControllerInput = 0;
				SelectButton(lastSelected);
				mSoundSys->playSound(B_RETURN, 30);
			}
			if (mControllers[i]->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)
			{
				timeBetweenControllerInput = 0;
				nextSubMenu = sm_Main;
				lastSelected = "bStart";
				mSoundSys->playSound(B_RETURN, 30);
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

			wantsToSetUpLabels = true;

			char buffer[32];
			sprintf_s(buffer,"addPlayers %d", numLocalPlayers);
			mGameClient->SendString(buffer);

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