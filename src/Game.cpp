#include "Game.h"
 
Game::Game(void)
{
	myShip = Car();
}
 
Game::~Game(void)
{
}
 
//-------------------------------------------------------------------------------------
void Game::createScene(void)
{
	//Try to load the test scene file, if it doesnt load properly load the backup scene
	parseDotScene("test.scene","General",mSceneMgr);


	//Setting a ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f, 1.0f));

	Ogre::Light* light = mSceneMgr->createLight("MainLight");
	light->setPosition(Ogre::Vector3(0.0f, 10.0f, 0.0f));

	Ogre::SceneNode* controllerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("controllerNode");

	//Create a game object thing
	myShip = Car("myShip", controllerNode);
	myShip.GetSceneNode()->setPosition(0,0,0);
	Ogre::Entity* myShipEnt = mSceneMgr->createEntity("shipEnt", "BoltCar.mesh");
	myShip.GetSceneNode()->attachObject(myShipEnt);

	Ogre::SceneNode* camNode = controllerNode->createChildSceneNode("camNode");
	camNode->attachObject(mCamera);

	camNode->translate(Ogre::Vector3(0.0f, 35.0f, 7.0f));

	mCamera->lookAt(myShip.GetSceneNode()->getPosition());
}

bool Game::keyPressed( const OIS::KeyEvent &arg )
{
	//Call the base class
	BaseApplication::keyPressed(arg);

	//Slightly move the ship when you press the button
	if(arg.key == OIS::KC_P)
	{
		//Replace this with actual physics and move somewhere else so that we can have continoius updates
		myShip.MoveForward(1.0f);
	}

	return true;
}
 
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        Game app;
 
        try 
        {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR| MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
#endif
        }
 
        return 0;
    }
 
#ifdef __cplusplus
}
#endif