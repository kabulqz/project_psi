#pragma once

#include "Settings.hpp"

class SoundManager
{
private:
	//Enum to categorize sound effects
	enum sfxType
	{
		UI,
		ENVIRONMENT,
		ALERT,
		MUSIC
	};
	std::map<std::string, sf::SoundBuffer> soundBuffers;
	std::map<std::string, std::unique_ptr<sf::Music>> musicTracks;
	std::map<std::string, sfxType> soundTypes;
	void playSoundInternal(const std::string& soundName);
	void loadSound(const std::string& name, const std::string& filePath, sfxType type);
	//values from 0 -1
	float& generalVolume;
	//values from 0 - 100
	float& uiVolume;
	float& environmentVolume;
	float& alertVolume;
	float& musicVolume;
public:
	SoundManager(float& generalAudio, float& uiAudio, float& environmentAudio, float& alertAudio, float& musicAudio);
	void loadSounds();
	void playSound(const std::string& soundName);
};