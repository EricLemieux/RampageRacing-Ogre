#include "Sound.h"

SoundSystem::SoundSystem(){}
SoundSystem::~SoundSystem()
{
	unsigned int count = sounds.size();

	for (unsigned int i = 0; i < count; ++i)
	{
		result = sounds[i].sound->release();
	}
}

void SoundSystem::initSound(){
	result = FMOD::System_Create(&system);

	result = system->getVersion(&version);

	result = system->init(32, FMOD_INIT_NORMAL, 0);

	
	for (int i = 0; i < 32; ++i){
		FMOD::Channel* ch;
		channels.push_back(ch);
	}

	//Read the sounds file
	Ogre::String fileName = "game.sounds";	//TODO figure out if we are storing all the sounds in the files, for now they are all in this one file
	Ogre::String basename, path;
	Ogre::StringUtil::splitFilename(fileName, basename, path);

	Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource(basename, "General");

	Ogre::String data = pStream->getLine();

	int soundCount = 0;

	while (!pStream->eof())
	{
		//Ignore comments
		if (data.c_str()[0] != '#')
		{
			char soundFile[256] = "";
			int loop = 0;
			sscanf_s(data.c_str(), "%s %i", &soundFile, 256, &loop);

			sounds.push_back(Sound());

			result = system->createSound(soundFile, FMOD_DEFAULT, 0, &sounds[soundCount].sound);

			//Looping?
			if (loop == 0){
				result = sounds[soundCount].sound->setMode(FMOD_LOOP_OFF);
			} else {
				result = sounds[soundCount].sound->setMode(FMOD_LOOP_OFF);
			}
			soundCount++;
		}


		data = pStream->getLine();
	}
	int a = 0;
}
void SoundSystem::update(){
	result = system->update();

	unsigned int ms = 0;
	unsigned int lenms = 0;
	bool         playing = 0;
	bool         paused = 0;
	int          channelsplaying = 0;

	for (int i = 0; i < channels.size(); ++i){
		if (channels[i])
		{
			FMOD::Sound *currentsound = 0;

			result = channels[i]->isPlaying(&playing);


			result = channels[i]->getPaused(&paused);


			result = channels[i]->getPosition(&ms, FMOD_TIMEUNIT_MS);


			channels[i]->getCurrentSound(&currentsound);
			if (currentsound)
			{
				result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
			}
		}
	}
	system->getChannelsPlaying(&channelsplaying);
	

}

void SoundSystem::playSound(SOUND_TYPE soundType, int chanType){
	bool playing = true;
	result = channels[(int) chanType]->isPlaying(&playing);
	if (soundType != CAR_STEADY && soundType != BGM){
		result = system->playSound(FMOD_CHANNEL_FREE, sounds[soundType].sound, false, &channels[chanType]);
	}
	else if (!playing){
		if (chanType == BGM){
			channels[chanType]->setPaused(true);
			channels[chanType]->setVolume(0.3f);
			channels[chanType]->setPaused(false);
		}
		result = system->playSound(FMOD_CHANNEL_FREE, sounds[soundType].sound, false, &channels[chanType]);
	}
}


void SoundSystem::pauseSound(int chanType){
	result = channels[chanType]->setPaused(true);
}