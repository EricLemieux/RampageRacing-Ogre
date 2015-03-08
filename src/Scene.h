#pragma once

#include <memory>
#include <vector>
#include <list>

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
#include <OISJoyStick.h>

#include "Physics.h"

#include "DotScene.h"

#include "GameObject.h"
#include "Car.h"
#include "TriggerVolume.h"
#include "CarCollisionTest.h"

#include "OBJ.h"

#include "Client.h"

#include "Controller.h"

#include "ItemBox.h"
#include "Missile.h"
#include "Mine.h"

#include <OgreOverlayContainer.h>
#include <OgreOverlayManager.h>

//Bad practice but works
static unsigned int numLocalPlayers = 1;


class Scene
{
public:
	Scene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Client> client, std::shared_ptr<Ogre::Camera> cameras[4], std::shared_ptr<Ogre::RenderWindow> window, std::shared_ptr<Controller> controllers[4]);
	~Scene();

	virtual bool Update();

	void ClearScene();

	virtual void LoadLevel(Ogre::String LevelName);

	virtual void KeyPressed(const OIS::KeyEvent &arg);
	virtual void KeyReleased(const OIS::KeyEvent &arg);

	virtual void mouseMoved(const OIS::MouseEvent &arg);
	virtual void mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual void mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	virtual void ControllerInput();

	virtual void AddCarToScene(Ogre::String name);
	virtual void AddTriggerVolumesToScene();

	virtual void SetUpViewports();

	inline std::shared_ptr<Ogre::SceneManager> GetSceneManager(){ return mSceneMgr; }
	inline std::shared_ptr<Ogre::Camera> GetCamera(){ return mCamera; }
	inline std::shared_ptr<PhysicsWorld> GetPhysicsWorld(){ return mPhysicsWorld; }

	inline bool GetIfShouldSwapScenes(){ return swapToTheNewScene; }
	//Don't fucking call this unless you know what you are doing... yeah, you...
	inline std::shared_ptr<Scene> GetNewScene(){ swapToTheNewScene = false; return newScene; }

protected:

	btClock clock;
	float timeStep;

	bool swapToTheNewScene = false;
	std::shared_ptr<Scene> newScene;

	std::shared_ptr<Client> mGameClient;

	std::shared_ptr<Ogre::SceneManager> mSceneMgr;

	std::shared_ptr<Ogre::Camera> mCameras[4];
	std::shared_ptr<Ogre::Camera> mCamera;

	std::shared_ptr<PhysicsWorld> mPhysicsWorld;

	std::shared_ptr<Ogre::RenderWindow> mWindow;

	std::vector<GameObject*> mObjects;

	std::shared_ptr<Controller> mControllers[4];

	void ResetCamera(void);

	void SwapToMainMenu();
	void SwapToGameplayLevel(Ogre::String levelName);
};

class GameplayScene : public Scene
{
public:
	GameplayScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Client> client, std::shared_ptr<Ogre::Camera> cameras[4], std::shared_ptr<Ogre::RenderWindow> window, std::shared_ptr<Controller> controllers[4]);
	~GameplayScene();

	virtual bool Update();

	virtual void LoadLevel(Ogre::String LevelName);

	virtual void KeyPressed(const OIS::KeyEvent &arg);
	virtual void KeyReleased(const OIS::KeyEvent &arg);

	virtual void mouseMoved(const OIS::MouseEvent &arg);
	virtual void mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual void mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	virtual void ControllerInput();

	virtual void AddCarToScene(Ogre::String name);
	virtual void AddTriggerVolumesToScene();

	void myTickCallback(btDynamicsWorld *world, btScalar timeStep);

	virtual void SetUpViewports();

	void SetUpItemBoxes();

	void FireMissile(int carID);
	void DropMine(int carID);

private:
	std::shared_ptr<Car> mCars[4];
	std::shared_ptr<Car> mCar;
	btRigidBody* body;

	ContactSensorCallback* callback;
	//currently 3 because 2 checkpoints and 1 finish line
	TriggerVolume* mTriggerVolumes[3];

	std::list<std::shared_ptr<GameObject>> mActiveWeapons;
	std::vector<std::shared_ptr<ItemBox>> mItemBoxes;
};

class MenuScene : public Scene
{
public:
	MenuScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Client> client, std::shared_ptr<Ogre::Camera> cameras[4], std::shared_ptr<Ogre::RenderWindow> window, std::shared_ptr<Controller> controllers[4]);
	~MenuScene();

	virtual bool Update();

	virtual void KeyPressed(const OIS::KeyEvent &arg);
	virtual void KeyReleased(const OIS::KeyEvent &arg);

	virtual void mouseMoved(const OIS::MouseEvent &arg);
	virtual void mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual void mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	virtual void ControllerInput();

	virtual void LoadLevel(Ogre::String levelName);
private:
	enum subMenus
	{
		sm_Main = 0,
		sm_PlayerCount,
		sm_LevelSelect
	};

	subMenus currentSubMenu;
	subMenus nextSubMenu;

	float deltaT;

	Ogre::Vector3 GetCamPosFromSubMenu(int subMenu);
	Ogre::Vector3 GetCamTargetFromSubMenu(int subMenu);

	Ogre::String mCurrentSelectedLevel;
};

class IntroScene : public Scene
{
public:
	IntroScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Client> client, std::shared_ptr<Ogre::Camera> cameras[4], std::shared_ptr<Ogre::RenderWindow> window, std::shared_ptr<Controller> controllers[4]);
	~IntroScene();

	virtual bool Update();

	inline virtual void KeyPressed(const OIS::KeyEvent &arg){ SwapToMainMenu(); }
	inline virtual void KeyReleased(const OIS::KeyEvent &arg){}
	
	inline virtual void mouseMoved(const OIS::MouseEvent &arg){ SwapToMainMenu(); }
	inline virtual void mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id){}
	inline virtual void mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id){}

	virtual void ControllerInput(){}
	
	virtual void LoadLevel(Ogre::String levelName);

private:
	float mTime = 0.0f;
};