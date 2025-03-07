#pragma once

#include "Settings.hpp"
#include "Card.hpp"
#include "Hero.hpp"
#include "AbilityTree.hpp"
#include "MapGeneration.hpp"

// OpenSSL
#include <openssl/evp.h>
#include <openssl/rand.h>

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
	int* level;
	std::vector<sf::Vector2i> path;
	std::shared_ptr<AbilityTree> abilityTree;
	BoardGamePlayer* player;
	std::vector<BoardGameEnemy> boardEnemies;
	std::vector<uint_least32_t> deck;		// saved deck we have
	// saved card collection we have

	std::string serializePath() const;
	std::string serializeLevel() const;
	static std::vector<sf::Vector2i> deserializePath(const std::string& data);
	static int* deserializeLevel(const std::string& data);
public:
	Save();//default constructor, so new save
	Save(const Save& save);
	Save& operator=(const Save& save);
	~Save() {
		delete[] level;
	}

	//Save(save data type from saves array in main menu state);
	uint_least32_t getSeed() const { return seed; }
	void setSeed(uint_least32_t seed) { this->seed = seed; }
	int* getLevel() const { return level; }
	std::vector<sf::Vector2i>& getPath() { return path; }
	std::shared_ptr<AbilityTree> getAbilityTree() { return abilityTree; }
	void setAbilityTree(const std::shared_ptr<AbilityTree>& abilityTree) { this->abilityTree = abilityTree; }

	BoardGamePlayer* getPlayer() const { return this->player; }
	void setPlayer(BoardGamePlayer* player) { this->player = player; }

	std::vector<BoardGameEnemy>& getEnemies() { return boardEnemies; }
	void setEnemies(const std::vector<sf::Vector2i>& path)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(0, static_cast<int>(path.size() - 1));

		while (boardEnemies.size() < 3)
		{
			if (const int index = dist(gen); path[index] != player->getMapPosition() && std::find(boardEnemies.begin(), boardEnemies.end(), path[index]) == boardEnemies.end())
			{
				boardEnemies.emplace_back(path[index]);
			}
		}
	}
	void setEnemies(const std::vector<BoardGameEnemy>& enemies) { boardEnemies = enemies; }

	void write(int slot);	// write to specific slot
	void write() const;			// write to current slot
	Save& load(int slot);	// load from specific slot
	Save& load();			// load from current slot
	static std::optional<std::filesystem::file_time_type>  getLastWriteTime(int slot);

	std::vector <uint_least32_t> getDeck() const { return deck; }
};