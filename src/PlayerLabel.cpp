#include "PlayerLabel.h"

PlayerLabel::PlayerLabel(Ogre::SceneNode* sceneNode, Ogre::Entity* checkBoxEnt, Ogre::MovableText* text)
{
	mSceneNode = sceneNode;

	mCheckBox = checkBoxEnt;
	mCheckBox->setVisible(false);
	mSceneNode->attachObject(mCheckBox);

	mText = text;
	mSceneNode->attachObject(mText);
}

PlayerLabel::~PlayerLabel()
{
}

void PlayerLabel::SetReadyToPlay(bool ready)
{
	mReadyToPlay = ready;
	mCheckBox->setVisible(ready);
}