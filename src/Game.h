/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.h
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
#ifndef __TutorialApplication_h_
#define __TutorialApplication_h_

#include "BaseApplication.h"
#include "DotScene.h"

#include "Car.h"
#include "GameObject.h"

//#include "GameObject.h"

class Game : public BaseApplication
{
public:
    Game(void);
    virtual ~Game(void);

protected:
    virtual void createScene(void);

	bool keyPressed( const OIS::KeyEvent &arg );

public:
	Ogre::SceneNode* shipNode;

	Car myShip;
};

#endif // #ifndef __TutorialApplication_h_
