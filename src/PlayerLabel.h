#pragma once

#include <Ogre.h>
#include "MovableText.h"

#include <memory>

class PlayerLabel
{
public:
	PlayerLabel(Ogre::SceneNode* sceneNode, Ogre::Entity* checkBoxEnt, Ogre::MovableText* text);
	~PlayerLabel();

	void SetReadyToPlay(bool ready);
	inline bool GetReadyToPlay(){ return mReadyToPlay; };

private:
	Ogre::SceneNode* mSceneNode;
	Ogre::MovableText* mText;
	Ogre::Entity* mCheckBox;

	bool mReadyToPlay = false;

	Ogre::String mMeshName;
};