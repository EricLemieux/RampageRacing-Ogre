#pragma once

#include "GameObject.h"

class TriggerVolume : public GameObject
{
public:
	TriggerVolume(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager);
	~TriggerVolume();
};