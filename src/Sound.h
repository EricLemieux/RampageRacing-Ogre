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

enum SOUND_TYPE{
	MEOW = 0,
	B_RETURN,
	B_SELECT,
	COUNTDOWN,
	CAR_ACCEL,
	CAR_STEADY,
	ITEM_EXPLODE,
	ITEM_PICKUP,
	BGM,
};

struct Sound
{
	FMOD::Sound* sound;
	int channel;
};

class SoundSystem
{
public:
	SoundSystem();
	~SoundSystem();

	void initSound();

	void update();

	void playSound(SOUND_TYPE soundType, int chanIndex);
	void pauseSound(int chanIndex);

	

private:
	FMOD::System     *system;
	std::vector<Sound> sounds;
	std::vector<FMOD::Channel*> channels;
	FMOD_RESULT       result;
	unsigned int      version;
	void             *extradriverdata = 0;
};