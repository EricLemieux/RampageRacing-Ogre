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

class Sound
{
public:
	Sound();
	~Sound();

	void initSound();

	void update();

	void playSound();
	void pauseSound();


private:
	FMOD::System     *system;
	FMOD::Sound      *sound1, *sound2, *sound3;
	FMOD::Channel    *channel = 0;
	FMOD_RESULT       result;
	unsigned int      version;
	void             *extradriverdata = 0;
};