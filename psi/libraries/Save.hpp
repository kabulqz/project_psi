#pragma once

#include "Settings.hpp"
#include "Card.hpp"
#include "Hero.hpp"
#include "AbilityTree.hpp"
#include "MapGeneration.hpp"

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

	Card* tempCard;
private:
	int slot;

	uint_least32_t  seed;
	int* level;
	std::vector<sf::Vector2i> path;
	std::shared_ptr<AbilityTree> abilityTree;
	BoardGamePlayer* player;
	// saved card deck
	// saved card collection we have
	//TypeOfMapGeneration mapGenerationType;

	std::string serializePath() const;
	std::string serializeLevel() const;
	static std::vector<sf::Vector2i> deserializePath(const std::string& data);
	static int* deserializeLevel(const std::string& data);
public:
	Save();//default constructor, so new save
	Save(const Save& save);
	Save& operator=(const Save& save);
	~Save();

	//Save(save data type from saves array in main menu state);
	uint_least32_t getSeed() const { return seed; }
	void setSeed(uint_least32_t seed) { this->seed = seed; }
	int* getLevel() const { return level; }
	std::vector<sf::Vector2i>& getPath() { return path; }
	std::shared_ptr<AbilityTree> getAbilityTree() { return abilityTree; }
	void setAbilityTree(const std::shared_ptr<AbilityTree>& abilityTree) { this->abilityTree = abilityTree; }

	BoardGamePlayer* getPlayer() const { return this->player; }
	void setPlayer(BoardGamePlayer* player) { this->player = player; }

	void write(int slot);	// write to specific slot
	void write() const;			// write to current slot
	Save& load(int slot);	// load from specific slot
	Save& load();			// load from current slot
	static std::optional<std::filesystem::file_time_type>  getLastWriteTime(int slot);
};

inline Save::~Save()
{
	delete[] level;
}