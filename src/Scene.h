#pragma once

#include <memory>
#include <vector>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include "Physics.h"

#include "DotScene.h"

#include "GameObject.h"
#include "Car.h"
#include "TriggerVolume.h"
#include "CarCollisionTest.h"

#include "OBJ.h"

#include "Client.h"

class Scene
{
public:
	Scene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera, std::shared_ptr<Client> client);
	~Scene();

	virtual bool Update();

	void ClearScene();

	virtual void LoadLevel(Ogre::String LevelName);

	virtual void KeyPressed(const OIS::KeyEvent &arg);
	virtual void KeyReleased(const OIS::KeyEvent &arg);

	virtual void mouseMoved(const OIS::MouseEvent &arg);
	virtual void mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual void mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	virtual void AddCarToScene(Ogre::String name);
	virtual void AddTriggerVolumesToScene();

	inline std::shared_ptr<Ogre::SceneManager> GetSceneManager(){ return mSceneMgr; }
	inline std::shared_ptr<Ogre::Camera> GetCamera(){ return mCamera; }
	inline std::shared_ptr<PhysicsWorld> GetPhysicsWorld(){ return mPhysicsWorld; }

	inline bool GetIfShouldSwapScenes(){ return swapToTheNewScene; }
	//Don't fucking call this unless you know what you are doing... yeah, you...
	inline std::shared_ptr<Scene> GetNewScene(){ swapToTheNewScene = false; return newScene; }

protected:
	bool swapToTheNewScene = false;
	std::shared_ptr<Scene> newScene;

	std::shared_ptr<Client> mGameClient;

	std::shared_ptr<Ogre::SceneManager> mSceneMgr;
	std::shared_ptr<Ogre::Camera> mCamera;

	std::shared_ptr<PhysicsWorld> mPhysicsWorld;

	std::vector<GameObject*> mObjects;

	void ResetCamera(void);
};

class GameplayScene : public Scene
{
public:
	GameplayScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera, std::shared_ptr<Client> client);
	~GameplayScene();

	virtual bool Update();

	virtual void LoadLevel(Ogre::String LevelName);

	virtual void KeyPressed(const OIS::KeyEvent &arg);
	virtual void KeyReleased(const OIS::KeyEvent &arg);

	virtual void mouseMoved(const OIS::MouseEvent &arg);
	virtual void mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual void mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	virtual void AddCarToScene(Ogre::String name);
	virtual void AddTriggerVolumesToScene();
private:
	Car* mCar;
	btRigidBody* body;

	ContactSensorCallback* callback;
	//currently 3 because 2 checkpoints and 1 finish line
	TriggerVolume* mTriggerVolumes[3];
};

class MenuScene : public Scene
{
public:
	MenuScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera, std::shared_ptr<Client> client);
	~MenuScene();

	virtual bool Update();

	virtual void KeyPressed(const OIS::KeyEvent &arg);
	virtual void KeyReleased(const OIS::KeyEvent &arg);

	virtual void mouseMoved(const OIS::MouseEvent &arg);
	virtual void mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual void mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	virtual void LoadLevel(Ogre::String levelName);
private:
	enum subMenus
	{
		sm_Main = 0,
		sm_LevelSelect
	};

	subMenus currentSubMenu;
	subMenus nextSubMenu;

	float deltaT;

	Ogre::Vector3 GetCamPosFromSubMenu(int subMenu);
	Ogre::Vector3 GetCamTargetFromSubMenu(int subMenu);
};