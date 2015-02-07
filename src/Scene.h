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

class Scene
{
public:
	Scene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera);
	~Scene();

	virtual bool Update();

	void ClearScene();

	void LoadSceneFile(Ogre::String fileName);

	virtual void KeyPressed(const OIS::KeyEvent &arg);
	virtual void KeyReleased(const OIS::KeyEvent &arg);

	virtual void AddCarToScene(Ogre::String name);

	inline std::shared_ptr<Ogre::SceneManager> GetSceneManager(){ return mSceneMgr; }
	inline std::shared_ptr<Ogre::Camera> GetCamera(){ return mCamera; }
	inline std::shared_ptr<PhysicsWorld> GetPhysicsWorld(){ return mPhysicsWorld; }

	inline bool GetIfShouldSwapScenes(){ return swapToTheNewScene; }
	//Don't fucking call this unless you know what you are doing... yeah, you...
	inline std::shared_ptr<Scene> GetNewScene(){ swapToTheNewScene = false; return newScene; }

protected:
	bool swapToTheNewScene = false;
	std::shared_ptr<Scene> newScene;

private:
	std::shared_ptr<Ogre::SceneManager> mSceneMgr;
	std::shared_ptr<Ogre::Camera> mCamera;

	std::shared_ptr<PhysicsWorld> mPhysicsWorld;

	std::vector<GameObject*> mObjects;

	void ResetCamera(void);
};

class GameplayScene : public Scene
{
public:
	GameplayScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera);
	~GameplayScene();

	virtual bool Update();

	virtual void KeyPressed(const OIS::KeyEvent &arg);
	virtual void KeyReleased(const OIS::KeyEvent &arg);

	virtual void AddCarToScene(Ogre::String name);
private:
	Car* mCar;
};

class MenuScene : public Scene
{
public:
	MenuScene(std::shared_ptr<Ogre::SceneManager> sceneMgr, std::shared_ptr<Ogre::Camera> camera);
	~MenuScene();

	virtual bool Update();

	virtual void KeyPressed(const OIS::KeyEvent &arg);
	virtual void KeyReleased(const OIS::KeyEvent &arg);
private:
};