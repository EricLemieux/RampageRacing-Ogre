#pragma once

#include "Scene.h"

class ScoreScreen : public Scene
{
public:
	ScoreScreen(SceneArguments args);
	~ScoreScreen();

	virtual bool Update();

	inline virtual void KeyPressed(const OIS::KeyEvent &arg){ SwapToMainMenu(); }
	inline virtual void KeyReleased(const OIS::KeyEvent &arg){}

	inline virtual void mouseMoved(const OIS::MouseEvent &arg){}
	inline virtual void mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id){}
	inline virtual void mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id){}

	virtual void ControllerInput();

	virtual void LoadLevel(Ogre::String levelName);
};