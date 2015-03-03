#include "Game.h"
 
Game::Game(void) : BaseApplication()
{
	mGameClient = std::shared_ptr<Client>(new Client());

	mGameClient->Connect("localhost", 8080, 8081, "password");
	mGameClient->SendString("connect");
}
 
Game::~Game(void)
{
}
 
//-------------------------------------------------------------------------------------
void Game::createScene(void)
{
	//Set up the current scene
	//mCurrentScene = std::unique_ptr<IntroScene>(new IntroScene(mSceneMgr, mGameClient, mWindow));
	mCurrentScene = std::unique_ptr<MenuScene>(new MenuScene(mSceneMgr, mGameClient, mWindow));

	mCurrentScene->LoadLevel("MainMenu");
}

bool Game::keyPressed( const OIS::KeyEvent &arg )
{
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

bool Game::mouseMoved(const OIS::MouseEvent &arg)
{
	BaseApplication::mouseMoved(arg);

	mCurrentScene->mouseMoved(arg);

	return true;
}
bool Game::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	BaseApplication::mousePressed(arg, id);

	mCurrentScene->mousePressed(arg, id);

	return true;
}
bool Game::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	BaseApplication::mouseReleased(arg, id);

	mCurrentScene->mouseReleased(arg, id);

	return true;
}

bool Game::axisMoved(const OIS::JoyStickEvent &arg, int axis)
{
	BaseApplication::axisMoved(arg, axis);

	mCurrentScene->axisMoved(arg, axis);

	return true;
}
bool Game::buttonPressed(const OIS::JoyStickEvent &arg, int button)
{
	BaseApplication::buttonPressed(arg, button);

	mCurrentScene->buttonPressed(arg, button);

	return true;
}
bool Game::buttonReleased(const OIS::JoyStickEvent &arg, int button)
{
	BaseApplication::buttonReleased(arg, button);

	mCurrentScene->buttonReleased(arg, button);

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