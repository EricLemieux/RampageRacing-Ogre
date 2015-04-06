#include "Scene.h"

ScoreScreen::ScoreScreen(SceneArguments args) : Scene(args)
{

}

ScoreScreen::~ScoreScreen()
{

}

void ScoreScreen::ControllerInput()
{

}

bool ScoreScreen::Update()
{
	return true;
}

void ScoreScreen::LoadLevel(Ogre::String levelName)
{
	ResetCamera();

	Ogre::DotSceneLoader a;
	a.parseDotScene(levelName + ".scene", "General", mSceneMgr.get());

	labels.clear();

	for (unsigned int i = 0; i < mGameClient->raceResults.size(); ++i)
	{
		//make the node
		char name[128];
		sprintf_s(name, 128, "playerResult%i", i);

		if (mSceneMgr->hasSceneNode(name))
			mSceneMgr->destroySceneNode(name);

		Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);

		Ogre::Entity* ent = mSceneMgr->createEntity("checkBox.mesh");

		//Make the text
		char words[128];
		sprintf_s(words, 128, "Player %i = %d:%d.%d", i + 1 + mGameClient->startingIndex, mGameClient->raceResults[i].minutes, mGameClient->raceResults[i].seconds, mGameClient->raceResults[i].ms);

		Ogre::MovableText* text = new Ogre::MovableText(name, words);
		text->setTextAlignment(Ogre::MovableText::H_LEFT, Ogre::MovableText::V_CENTER);
		node->setPosition(-3, 1.5 - (i*0.6), -5);
		node->scale(1, 0.5, 1);

		std::shared_ptr<PlayerLabel> p = std::shared_ptr<PlayerLabel>(new PlayerLabel(node, ent, text));

		labels.push_back(p);
	}
}