/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#include "BaseApplication.h"

//-------------------------------------------------------------------------------------
BaseApplication::BaseApplication(void)
    : mRoot(0),
    mSceneMgr(0),
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mTrayMgr(0),
    mCursorWasVisible(false),
    mShutDown(false),
    mInputManager(0),
    mMouse(0),
    mKeyboard(0)
{
}

//-------------------------------------------------------------------------------------
BaseApplication::~BaseApplication(void)
{
    if (mTrayMgr) delete mTrayMgr;

    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow.get(), this);
    windowClosed(mWindow.get());
}

//-------------------------------------------------------------------------------------
bool BaseApplication::configure(void)
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if(mRoot->restoreConfig())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = std::shared_ptr<Ogre::RenderWindow>(mRoot->initialise(true, "Rampage Racing"));

        return true;
    }
    else
    {
        return false;
    }
}
//-------------------------------------------------------------------------------------
void BaseApplication::chooseSceneManager(void)
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = std::shared_ptr<Ogre::SceneManager>(mRoot->createSceneManager(Ogre::ST_GENERIC));
	mOverlaySystem = new Ogre::OverlaySystem();
	mSceneMgr->addRenderQueueListener(mOverlaySystem);
}
//-------------------------------------------------------------------------------------
void BaseApplication::createCamera(void)
{
   // // Create the camera
   // mCamera = std::shared_ptr<Ogre::Camera>(mSceneMgr->createCamera("PlayerCam"));
   //
   // // Position it at 500 in Z direction
   // mCamera->setPosition(Ogre::Vector3(0,0,80));
   // // Look back along -Z
   // mCamera->lookAt(Ogre::Vector3(0,0,-300));
   // mCamera->setNearClipDistance(0.1);
}
//-------------------------------------------------------------------------------------
void BaseApplication::createFrameListener(void)
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem( pl );

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

	if (mInputManager->getNumberOfDevices(OIS::OISJoyStick) > 0)
	{
		mJoysticks.resize(mInputManager->getNumberOfDevices(OIS::OISJoyStick));

		mItJoystick = mJoysticks.begin();
		mItJoystickEnd = mJoysticks.end();
		for (;mItJoystick != mItJoystickEnd; ++mItJoystick)
		{
			(*mItJoystick) = static_cast<OIS::JoyStick*>(mInputManager->createInputObject(OIS::OISJoyStick, true));
			(*mItJoystick)->setEventCallback(this);
		}
	}

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    //Set initial mouse clipping size
    windowResized(mWindow.get());

    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow.get(), this);

	OgreBites::InputContext inputContext;
	inputContext.mMouse = mMouse; 
	inputContext.mKeyboard = mKeyboard;
    mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow.get(), inputContext, this);
   // mTrayMgr->hideCursor();

    mRoot->addFrameListener(this);
}
//-------------------------------------------------------------------------------------
void BaseApplication::destroyScene(void)
{
}
//-------------------------------------------------------------------------------------
void BaseApplication::createViewports(void)
{
// // Create one viewport, entire window
// Ogre::Viewport* vp = mWindow->addViewport(mCamera.get());
// vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
//
// // Alter the camera aspect ratio to match the viewport
// mCamera->setAspectRatio(
//     Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}
//-------------------------------------------------------------------------------------
void BaseApplication::setupResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}
//-------------------------------------------------------------------------------------
void BaseApplication::createResourceListener(void)
{

}
//-------------------------------------------------------------------------------------
void BaseApplication::loadResources(void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//-------------------------------------------------------------------------------------
void BaseApplication::go(void)
{
#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

    if (!setup())
        return;

	while(!mShutDown)
	{
		Update();

		Render();
	}

    // clean up
    destroyScene();
}
//-------------------------------------------------------------------------------------
bool BaseApplication::setup(void)
{
	mRoot = std::shared_ptr<Ogre::Root>(new Ogre::Root(mPluginsCfg));

    setupResources();

    bool carryOn = configure();
    if (!carryOn) return false;

    chooseSceneManager();
    createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();

    // Create the scene
    createScene();

    createFrameListener();

    return true;
};
//-------------------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;

    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();

	if (mInputManager->getNumberOfDevices(OIS::OISJoyStick) > 0)
	{
		mItJoystick = mJoysticks.begin();
		mItJoystickEnd = mJoysticks.end();
		for (; mItJoystick != mItJoystickEnd; ++mItJoystick)
		{
			(*mItJoystick)->capture();
		}
	}

    mTrayMgr->frameRenderingQueued(evt);

    if (!mTrayMgr->isDialogVisible())
    {
    }

    return true;
}
//-------------------------------------------------------------------------------------
bool BaseApplication::keyPressed( const OIS::KeyEvent &arg )
{
    if (mTrayMgr->isDialogVisible()) return true;   // don't process any more keys if dialog is up

    if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
    {
        mTrayMgr->toggleAdvancedFrameStats();
    }
    else if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
    {
        Ogre::TextureFilterOptions tfo;
        unsigned int aniso;

        Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
        Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
    }
    else if(arg.key == OIS::KC_F5)   // refresh all textures
    {
        Ogre::TextureManager::getSingleton().reloadAll();
    }
    else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
    {
        mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
    }
    else if (arg.key == OIS::KC_ESCAPE)
    {
        mShutDown = true;
    }

    return true;
}

bool BaseApplication::keyReleased( const OIS::KeyEvent &arg )
{
    return true;
}

bool BaseApplication::mouseMoved( const OIS::MouseEvent &arg )
{
    if (mTrayMgr->injectMouseMove(arg)) return true;
    return true;
}

bool BaseApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    if (mTrayMgr->injectMouseDown(arg, id)) return true;
    return true;
}

bool BaseApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    if (mTrayMgr->injectMouseUp(arg, id)) return true;
    return true;
}

//Adjust mouse clipping area
void BaseApplication::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow.get())
    {
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );
			
			if (mInputManager->getNumberOfDevices(OIS::OISJoyStick) > 0)
			{
				mItJoystick = mJoysticks.begin();
				mItJoystickEnd = mJoysticks.end();
				for (; mItJoystick != mItJoystickEnd; ++mItJoystick)
				{
					mInputManager->destroyInputObject(*mItJoystick);
				}
			}

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}

bool BaseApplication::Update()
{
	return true;
}

bool BaseApplication::Render()
{
	int a = mWindow->getNumViewports();
	int b = mSceneMgr->getCameras().size();

	mRoot->renderOneFrame();

	return true;
}

bool BaseApplication::axisMoved(const OIS::JoyStickEvent &arg, int axis)
{
	return true;
}
bool BaseApplication::buttonPressed(const OIS::JoyStickEvent &arg, int button)
{
	//mItJoystickListener = mJoystickListeners.begin();
	//mItJoystickListenerEnd = mJoystickListeners.end();
	//for (; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener) {
	//	if (!itJoystickListener->second->axisMoved(e, axis))
	//		break;
	//}

	//mItJoystick = mJoysticks.begin();
	//mItJoystickEnd = mJoysticks.end();
	//for (; mItJoystick != mItJoystickEnd; ++mItJoystick)
	//{
	//	if ()
	//	{
	//
	//	}
	//}

	return true;
}
bool BaseApplication::buttonReleased(const OIS::JoyStickEvent &arg, int button)
{
	return true;
}