#include "SoundManager.hpp"

SoundManager::SoundManager(float& generalAudio, float& environmentAudio, float& uiAudio, float& alertAudio, float& musicAudio) :
generalVolume(generalAudio), uiVolume(uiAudio), environmentVolume(environmentAudio), alertVolume(alertAudio), musicVolume(musicAudio)
{}

//Internal method to play sounds
void SoundManager::playSoundInternal(const std::string& soundName)
{
	//Set the volume level depending on the sound type
	sfxType type = soundTypes[soundName];
	if (type == MUSIC)
	{
		std::unique_ptr<sf::Music>& music = musicTracks[soundName];
		music->setVolume(generalVolume * musicVolume);

		music->play();
		std::cout << "Music \"" << soundName << "\" is playing\n";

		while(music->getStatus() == sf::Music::Playing)
		{
			sf::sleep(sf::milliseconds(100));
		}
	}
	else
	{
		float volumeLevel = 100.0f;
		sf::Sound sound;
		sound.setBuffer(soundBuffers[soundName]);

		// Set volume based on sound type
		switch (type)
		{
		case UI: volumeLevel = uiVolume; break;
		case ENVIRONMENT: volumeLevel = environmentVolume; break;
		case ALERT: volumeLevel = alertVolume; break;
		default: break;
		}

		sound.setVolume(generalVolume * volumeLevel);
		sound.play();
		std::cout << "Sound \"" << soundName << "\" has been played\n";

		while (sound.getStatus() == sf::Sound::Playing)
		{
			sf::sleep(sf::milliseconds(100));
		}
	}
}

void SoundManager::loadSound(const std::string& name, const std::string& filePath, sfxType type)
{
	if (type == MUSIC)
	{
		auto music = std::make_unique<sf::Music>();
		if (!music->openFromFile(filePath))
		{
			std::cerr << "Error loading music file " << filePath << "\n";
			return;
		}
		musicTracks[name] = std::move(music); // Store the unique_ptr in the map
	}
	else
	{
		sf::SoundBuffer buffer;
		if (!buffer.loadFromFile(filePath))
		{
			std::cerr << "Error loading sound file " << filePath << "\n";
			return;
		}
		soundBuffers[name] = buffer;
	}

	soundTypes[name] = type;
}

//Play a sound concurrently
void SoundManager::playSound(const std::string& soundName)
{
	if (soundBuffers.find(soundName) == soundBuffers.end())
	{
		std::cerr << "Sound not found: " << soundName << "\n";
		return;
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