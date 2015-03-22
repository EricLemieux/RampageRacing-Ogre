#include "Sound.h"

Sound::Sound(){}
Sound::~Sound(){
	result = sound1->release();
}

void Sound::initSound(){
	result = FMOD::System_Create(&system);

	result = system->getVersion(&version);

	/*auto c = Ogre::ResourceGroupManager::getSingleton()
		.findResourceLocation("General", "meow.mp3").get();
	std::string s;


	for (int i = 0; i < c->size(); ++i){
		s = c->at(i);
	}*/

	char* c = "..\\dist\\media\\sounds\\meow.mp3";

	result = system->init(32, FMOD_INIT_NORMAL, 0);
	
	result = system->createSound(c, FMOD_DEFAULT, 0, &sound1);

	result = sound1->setMode(FMOD_LOOP_NORMAL);
		
}
void Sound::update(){
	result = system->update();

	unsigned int ms = 0;
	unsigned int lenms = 0;
	bool         playing = 0;
	bool         paused = 0;
	int          channelsplaying = 0;

	if (channel)
	{
		FMOD::Sound *currentsound = 0;

		result = channel->isPlaying(&playing);


		result = channel->getPaused(&paused);
	

		result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);


		channel->getCurrentSound(&currentsound);
		if (currentsound)
		{
			result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
		}
	}

	system->getChannelsPlaying(&channelsplaying);
	

}

void Sound::playSound(){
	result = system->playSound(FMOD_CHANNEL_FREE, sound1, false, &channel);
}
void Sound::pauseSound(){
	result = channel->setPaused(true);
}