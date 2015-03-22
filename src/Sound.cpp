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

	FMOD::Channel* ch;
	channels.push_back(ch);

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
			char soundFile[256];
			int loop;
			sscanf_s(data.c_str(), "%s %i", soundFile, 256, loop);

			sounds.push_back(Sound());

			result = system->createSound(soundFile, FMOD_DEFAULT, 0, &sounds[soundCount].sound);

			//Looping?
			
			result = sounds[soundCount].sound->setMode(FMOD_LOOP_OFF);

			soundCount++;
		}


		data = pStream->getLine();
	}		
}
void SoundSystem::update(){
	result = system->update();

	unsigned int ms = 0;
	unsigned int lenms = 0;
	bool         playing = 0;
	bool         paused = 0;
	int          channelsplaying = 0;

	if (channels[0])
	{
		FMOD::Sound *currentsound = 0;

		result = channels[0]->isPlaying(&playing);


		result = channels[0]->getPaused(&paused);
	

		result = channels[0]->getPosition(&ms, FMOD_TIMEUNIT_MS);


		channels[0]->getCurrentSound(&currentsound);
		if (currentsound)
		{
			result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
		}
	}

	system->getChannelsPlaying(&channelsplaying);
	

}

void SoundSystem::playSound(){
	result = system->playSound(FMOD_CHANNEL_FREE, sounds[0].sound, false, &channels[0]);
}
void SoundSystem::pauseSound(){
	result = channels[0]->setPaused(true);
}