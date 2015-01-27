#include "Game.h"
 
Game::Game(void)
{
}
 
Game::~Game(void)
{
}
 
//-------------------------------------------------------------------------------------
void Game::createScene(void)
{
    //// Set the scene's ambient light
    //mSceneMgr->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.1f));
	//mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
 	//
    //// Create an Entity
    //Ogre::Entity* ogreHead = mSceneMgr->createEntity("Head", "ogrehead.mesh");
 	//
    //// Create a SceneNode and attach the Entity to it
    //Ogre::SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode");
    //headNode->attachObject(ogreHead);
	//ogreHead->setCastShadows(true);
	//
	//Ogre::Entity* Box = mSceneMgr->createEntity("Box", "cube.mesh");
    //Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Box");
    //boxNode->attachObject(Box);
	//Box->setCastShadows(true);
 	//
    //// Create a Light and set its position
    //Ogre::Light* light = mSceneMgr->createLight("MainLight");
    //light->setPosition(0.0f, 80.0f, 0.0f);
	//light->setDiffuseColour(Ogre::ColourValue(1.0f,0.0f,0.0f));
	//light->setDirection(0.0f, -1.0f, 0.0f);
	//light->setCastShadows(true);
	//
	//boxNode->setPosition(0.0f, -100.0f, 0.0f);

	parseDotScene("test.scene","General",mSceneMgr);

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