#pragma once

#include "fmod_errors.h"
#include "fmod.hpp"
#include <assert.h>


#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <vector>

class SoundSystem
{
public:
	SoundSystem();
	~SoundSystem();

	void initSound();

	void update();

	void playSound();
	void pauseSound();

	struct Sound
	{
		FMOD::Sound* sound;
		int channel;
	};

private:
	FMOD::System     *system;
	std::vector<Sound> sounds;
	std::vector<FMOD::Channel*> channels;
	FMOD_RESULT       result;
	unsigned int      version;
	void             *extradriverdata = 0;
};