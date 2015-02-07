#include "Game.h"
 
Game::Game(void) : BaseApplication()
{
}
 
Game::~Game(void)
{
}
 
//-------------------------------------------------------------------------------------
void Game::createScene(void)
{
	//Set up the current scene
	mCurrentScene = std::unique_ptr<MenuScene>(new MenuScene(mSceneMgr, mCamera));

	mCurrentScene->LoadSceneFile("MainMenu.scene");
}

bool Game::keyPressed( const OIS::KeyEvent &arg )
{
	//Call the base class
	//BaseApplication::keyPressed(arg);

	mCurrentScene->KeyPressed(arg);


	return true;
}

bool Game::keyReleased( const OIS::KeyEvent &arg )
{
	//Call the base class
	BaseApplication::keyReleased(arg);

	mCurrentScene->KeyReleased(arg);

	return true;
}

bool Game::Update()
{
	//Check to see if the current scene needs to be swapped
	if (mCurrentScene->GetIfShouldSwapScenes())
	{
		mCurrentScene = std::move(mCurrentScene->GetNewScene());
	}

	mCurrentScene->Update();

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