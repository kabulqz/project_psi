#pragma once

#include "Settings.hpp"
#include "Card.hpp"
#include "Hero.hpp"

// OpenSSL
#include <openssl/evp.h>
#include <openssl/rand.h>

enum class TypeOfMapGeneration
{
	FUTURE,
	CLASS,
	FOR,
	TYPE,
	OF,
	MAP,
	GENERATION

};

class Ability;
class AbilityTree;

class Save {
private:	// OpenSSL
	static constexpr size_t KEY_SIZE = 32;	// AES-256
	static constexpr size_t IV_SIZE = 16;	// Block size for AES
	std::vector<unsigned char> key;

	bool generateKey();
	bool encryptData(const std::string& plainText, std::vector<unsigned char>& cipherText, std::vector<unsigned char>& iv) const;
	bool decryptData(const std::vector<unsigned char>& cipherText, const std::vector<unsigned char>& iv, std::string& plainText) const;

	bool saveKeyToFile(const std::string& keyFilePath) const;
	bool loadKeyFromFile(const std::string& keyFilePath);
private:
	int slot;

	uint_least32_t  seed;
	std::shared_ptr<AbilityTree> abilityTree;
	Player* player;
	//TypeOfMapGeneration mapGenerationType;
public:
	Save();//default constructor, so new save
	Save(const Save& save);
	~Save() = default;
	void createPlayer(sf::Vector2i position);
	//Save(save data type from saves array in main menu state);
	uint_least32_t getSeed() { return seed; }
	void setSeed(uint_least32_t seed) { this->seed = seed; }
	std::shared_ptr<AbilityTree> getAbilityTree() { return abilityTree; }
	void setAbilityTree(std::shared_ptr<AbilityTree> abilityTree) { this->abilityTree = abilityTree; }

	Player* getPlayer() const { return this->player; }
	void setPlayer(Player* player) { this->player = player; }

	Save& operator=(const Save& save);
	void write(int slot);	// write to specific slot
	void write() const;			// write to current slot
	Save& load(int slot);	// load from specific slot
	Save& load();			// load from current slot
	static std::optional<std::filesystem::file_time_type>  getLastWriteTime(int slot);
};