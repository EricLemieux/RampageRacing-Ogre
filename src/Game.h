#pragma once

#include "BaseApplication.h"
#include "DotScene.h"

#include "Car.h"
#include "GameObject.h"

#include "Physics.h"

#include "Scene.h"

#include "Client.h"

#include "Controller.h"

class Game : public BaseApplication
{
public:
    Game(void);
    virtual ~Game(void);

protected:
    virtual void createScene(void);

	virtual bool Update();
	virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);

	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg );

	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

private:
	std::shared_ptr<Client> mGameClient;

	std::shared_ptr<Controller> mControllers[4];

public:
	std::shared_ptr<Scene> mCurrentScene;
};