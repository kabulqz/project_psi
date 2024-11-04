#include "SoundManager.hpp"

SoundManager::SoundManager(float& generalAudio, float& environmentAudio, float& uiAudio, float& alertAudio, float& musicAudio) :
generalVolume(generalAudio), uiVolume(uiAudio), environmentVolume(environmentAudio), alertVolume(alertAudio), musicVolume(musicAudio)
{}

//Internal method to play sounds
void SoundManager::playSoundInternal(const std::string& soundName)
{
	sf::Sound sound;
	sound.setBuffer(soundBuffers[soundName]);

	//Set the volume level depending on the sound type
	sfxType type = soundTypes[soundName];
	float volumeLevel = 100.0f; //Default value

	switch (type)
	{
	case UI:
		volumeLevel = uiVolume;
		break;
	case ENVIRONMENT:
		volumeLevel = environmentVolume;
		break;
	case ALERT:
		volumeLevel = alertVolume;
		break;
	case MUSIC:
		volumeLevel = musicVolume;
		break;
	}

	sound.setVolume(generalVolume * volumeLevel);
	sound.play();
	std::cout << "Sound \"" << soundName << "\" has been played\n";

	//Wait until the sound finishes playing
	while (sound.getStatus() == sf::Sound::Playing)
	{
		sf::sleep(sf::milliseconds(100));
	}
}

void SoundManager::loadSound(const std::string& name, const std::string& filePath, sfxType type)
{
	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile(filePath))
	{
		std::cerr << "Error loading sound file " << filePath << "\n";
		exit(-3);
	}
	soundBuffers[name] = buffer;
	soundTypes[name] = type; //Store the type of sound

}

//Play a sound concurrently
void SoundManager::playSound(const std::string& soundName)
{
	if (soundBuffers.find(soundName) == soundBuffers.end())
	{
		std::cerr << "Sound not found: " << soundName << "\n";
		exit(-3);
	}

	//Use thread to play the sound
	std::thread soundThread(&SoundManager::playSoundInternal, this, soundName);
	//Detach the thread to allow independent execution
	soundThread.detach();
}

//Load all sound buffers
void SoundManager::loadSounds()
{
	std::string path = "src/audio/";
	loadSound("mouseClick", path + "click3.ogg", UI);
	loadSound("Continue", path + "switch34.ogg", UI);
}