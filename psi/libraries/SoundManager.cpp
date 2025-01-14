#include "SoundManager.hpp"

SoundManager::SoundManager(float& generalAudio, float& uiAudio, float& ambienceAudio, float& alertAudio, float& musicAudio) :
generalVolume(generalAudio), uiVolume(uiAudio), ambienceVolume(ambienceAudio), alertVolume(alertAudio), musicVolume(musicAudio)
{}

void SoundManager::playSoundInternal(const std::string& soundName)
{
	std::lock_guard<std::mutex> lock(soundMutex); // Synchronize access to shared resources
	sfxType type = soundTypes[soundName];

	// Default to 1.0f volume for safety
	float typeVolume = 1.0f;

	// Determine the correct volume based on type
	switch (type)
	{
	case UI:
		typeVolume = uiVolume / 100.0f;  // Normalize UI volume (0-100 to 0-1)
		break;
	case AMBIENCE:
		typeVolume = ambienceVolume / 100.0f;  // Normalize Ambience volume (0-100 to 0-1)
		break;
	case ALERT:
		typeVolume = alertVolume / 100.0f;  // Normalize Alert volume (0-100 to 0-1)
		break;
	case MUSIC:
		typeVolume = musicVolume / 100.0f;  // Normalize Music volume (0-100 to 0-1)
		break;
	}

	// Apply the general volume after the type-specific volume calculation
	float finalVolume = typeVolume * generalVolume;  // Apply general volume here

	// Handle MUSIC separately (it's managed via sf::Music, not sf::Sound)
	if (type == MUSIC)
	{
		if (!musicTracks.contains(soundName))
		{
			std::cerr << "Music not found: " << color("ef233c", soundName) << "\n";
			return;
		}

		auto& music = musicTracks[soundName];
		music->setVolume(finalVolume * 100.0f);  // sf::Music expects volume in the 0-100 range
		music->play();

		std::cout << "Music " << gradient("ff4d6d", "ffccd5", soundName) << " is playing at volume: " << finalVolume * 100.0f << "\n";
	}
	else
	{
		if (!soundBuffers.contains(soundName))
		{
			std::cerr << "Sound buffer not found: " << color("ef233c", soundName) << "\n";
			return;
		}

		// Create a new sound and store it in the activeSounds map
		auto sound = std::make_unique<sf::Sound>();
		sound->setBuffer(soundBuffers[soundName]);
		sound->setVolume(finalVolume * 100.0f);  // sf::Sound expects volume in the 0-100 range
		sound->play();
		activeSounds[soundName] = std::move(sound); // Store the sound in the map

		std::cout << "Sound " << gradient("52b788", "d8f3dc", soundName) << " has been played at volume: " << finalVolume * 100.0f << "\n";
	}
}

// Load a sound or music track
void SoundManager::loadSound(const std::string& name, const std::string& filePath, sfxType type)
{
	if (type == MUSIC)
	{
		auto music = std::make_unique<sf::Music>();
		if (!music->openFromFile(filePath))
		{
			std::cerr << "Error loading music file: " << color("ef233c", filePath) << "\n";
			return;
		}
		musicTracks[name] = std::move(music);
	}
	else
	{
		sf::SoundBuffer buffer;
		if (!buffer.loadFromFile(filePath))
		{
			std::cerr << "Error loading sound file: " << color("ef233c", filePath) << "\n";
			return;
		}
		soundBuffers[name] = buffer;
	}

	soundTypes[name] = type;
}

// Play a sound or music track concurrently
void SoundManager::playSound(const std::string& soundName)
{
	if (!soundTypes.contains(soundName))
	{
		std::cerr << "Sound not found: " << color("ef233c", soundName) << "\n";
		return;
	}

	std::thread(&SoundManager::playSoundInternal, this, soundName).detach(); // Fire and forget
}

// Stop a currently playing sound
void SoundManager::stopSound(const std::string& soundName)
{
	std::lock_guard<std::mutex> lock(soundMutex);

	if (activeSounds.contains(soundName))
	{
		activeSounds[soundName]->stop();
		activeSounds.erase(soundName);
		std::cout << "Sound " << gradient("ffc300", "ffea00", soundName) << " has been stopped\n";
	}
	else
	{
		std::cerr << "Sound " << gradient("ffc300", "ffea00", soundName) << " is not currently playing\n";
	}
}

bool SoundManager::isSoundPlaying(const std::string& soundName)
{
	std::lock_guard<std::mutex> lock(soundMutex);

	// Check if it's a regular sound
	if (activeSounds.contains(soundName))
	{
		if (activeSounds[soundName]->getStatus() == sf::Sound::Playing)
		{
			return true;
		}
	}

	// Check if it's music
	if (musicTracks.contains(soundName))
	{
		if (musicTracks[soundName]->getStatus() == sf::Music::Playing)
		{
			return true;
		}
	}

	return false; // Not playing
}

//Load all sound buffers
void SoundManager::loadSounds()
{
	std::string path = "src/audio/";
	loadSound("Intro", path + "intro.ogg", UI);
	loadSound("MouseClick", path + "click3.ogg", UI);
	loadSound("Continue", path + "switch34.ogg", UI);
	loadSound("Transition", path + "transition.ogg", UI);
	loadSound("Ambience_crt", path + "ambience_crt.ogg", AMBIENCE);
	loadSound("DiceThrow", path + "dice-throw-3.ogg", ALERT);
	loadSound("AbilityUnlock", path + "ability-unlock.ogg", ALERT);
}