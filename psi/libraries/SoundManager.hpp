#pragma once

#include "Settings.hpp"

class SoundManager
{
private:
	// Enum to categorize sound effects
	enum sfxType
	{
		UI,
		ALERT,
		AMBIENCE,
		MUSIC
	};

	std::map<std::string, sf::SoundBuffer> soundBuffers;
	std::map<std::string, std::unique_ptr<sf::Sound>> activeSounds;
	std::map<std::string, std::unique_ptr<sf::Music>> musicTracks;
	std::map<std::string, sfxType> soundTypes;

	std::mutex soundMutex;

	void playSoundInternal(const std::string& soundName);
	void loadSound(const std::string& name, const std::string& filePath, sfxType type);

	// Volume values (references to external settings)
	float& generalVolume;
	float& uiVolume;
	float& ambienceVolume;
	float& alertVolume;
	float& musicVolume;

public:
	SoundManager(float& generalAudio, float& uiAudio, float& ambienceAudio, float& alertAudio, float& musicAudio);
	void loadSounds();
	void playSound(const std::string& soundName);
	void stopSound(const std::string& soundName);
	bool isSoundPlaying(const std::string& soundName);
};