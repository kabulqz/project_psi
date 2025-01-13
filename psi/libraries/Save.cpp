#include "Save.hpp"
#include "Card.hpp"

std::optional<std::filesystem::file_time_type>  Save::getLastWriteTime(int slot)
{
	try
	{
		std::filesystem::path filepath = "src/saves/save" + std::to_string(slot) + ".sav";
		if (std::filesystem::exists(filepath))
		{
			return std::filesystem::last_write_time(filepath);
		}
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
	}

	return std::nullopt;
}

bool Save::generateKey()
{
	try
	{
		key.resize(KEY_SIZE);
		if (!RAND_bytes(key.data(), KEY_SIZE))
		{
			std::cerr << "Error: RAND_bytes failed to generate key.\n";
			return false;
		}
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception in generateKey: " << e.what() << "\n";
		return false;
	}
}

bool Save::encryptData(const std::string& plainText, std::vector<unsigned char>& cipherText,
	std::vector<unsigned char>& iv) const
{
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!ctx) return false;

	iv.resize(IV_SIZE);
	if (!RAND_bytes(iv.data(), IV_SIZE))
	{
		std::cerr << "Failed to generate IV.\n";
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()))
	{
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	int len = 0;
	cipherText.resize(plainText.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
	if (!EVP_EncryptUpdate(ctx, cipherText.data(), &len, reinterpret_cast<const unsigned char*>(plainText.data()), static_cast<int>(plainText.size())))
	{
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	int cipherTextLen = len;

	if (!EVP_EncryptFinal_ex(ctx, cipherText.data() + len, &len))
	{
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	cipherTextLen += len;
	cipherText.resize(cipherTextLen);

	EVP_CIPHER_CTX_free(ctx);
	return true;
}

bool Save::decryptData(const std::vector<unsigned char>& cipherText, const std::vector<unsigned char>& iv,
	std::string& plainText) const
{
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!ctx) return false;

	if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()))
	{
		std::cerr << "Failed to initialize decryption.\n";
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	std::vector<unsigned char> plainTextBuf(cipherText.size() + EVP_MAX_BLOCK_LENGTH);
	int len = 0;
	if (!EVP_DecryptUpdate(ctx, plainTextBuf.data(), &len, cipherText.data(), static_cast<int>(cipherText.size())))
	{
		std::cerr << "Failed during decryption update.\n";
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	//
	int plainTextLen = len;
	if (!EVP_DecryptFinal_ex(ctx, plainTextBuf.data() + len, &len))
	{
		std::cerr << "Failed during decryption finalization.\n";
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	//
	plainTextLen += len;
	plainTextBuf.resize(plainTextLen);
	plainText.assign(plainTextBuf.begin(), plainTextBuf.end());

	EVP_CIPHER_CTX_free(ctx);
	return true;
}

bool Save::saveKeyToFile(const std::string& keyFilePath) const
{
	std::ofstream keyFile(keyFilePath, std::ios::binary);
	if (!keyFile.is_open()) return false;

	keyFile.write(reinterpret_cast<const char*>(key.data()), key.size());
	keyFile.close();
	return true;
}

bool Save::loadKeyFromFile(const std::string& keyFilePath)
{
	std::ifstream keyFile(keyFilePath, std::ios::binary);
	if (!keyFile.is_open()) return false;

	key.resize(KEY_SIZE);  // Ensure this size matches the size used during saving
	keyFile.read(reinterpret_cast<char*>(key.data()), KEY_SIZE);
	if (keyFile.gcount() != KEY_SIZE) return false;

	keyFile.close();
	return true;
}

std::string Save::serializePath() const
{
	std::ostringstream oss;

	for (size_t i = 0; i < path.size(); ++i)
	{
		oss << path[i].x << "," << path[i].y << ";";
	}

	return oss.str();
}

std::string Save::serializeLevel() const
{
	std::ostringstream oss;

	for (size_t i =0; i < WIDTH * HEIGHT; ++i)
	{
		oss << level[i];
	}

	return oss.str();
}

std::vector<sf::Vector2i> Save::deserializePath(const std::string& data)
{
	std::vector<sf::Vector2i> pathVector;
	std::istringstream iss(data);
	std::string token;

	while (std::getline(iss, token, ';'))
	{
		size_t commaPos = token.find(',');
		if (commaPos == std::string::npos)
		{
			throw std::runtime_error("Invalid path format: missing comma.");
		}

		int x = std::stoi(token.substr(0, commaPos));
		int y = std::stoi(token.substr(commaPos + 1));
		pathVector.emplace_back(x, y);
	}

	return pathVector;
}

int* Save::deserializeLevel(const std::string& data)
{
	if (data.size() != WIDTH * HEIGHT)
	{
		throw std::runtime_error("Invalid data size for level deserialization.");
	}

	const auto level = new int[WIDTH * HEIGHT];

	for (size_t i = 0; i < WIDTH * HEIGHT; ++i)
	{
		level[i] = data[i] - '0';
	}

	return level;
}

Save::Save()
{
	std::cout << gradient("ccff33", "38b000", "Creating new save.\n");
#ifdef DEBUG
	const std::string keyFilePath = "x64/Debug/encryption.key";
#else
	const std::string keyFilePath = "x64/Release/encryption.key";
#endif // DEBUG

	// Load the key from file if it exist, otherwise generate a new key and save it to file
	if (!std::filesystem::exists(keyFilePath))
	{
		if (!generateKey() || !saveKeyToFile(keyFilePath))
		{
			throw std::runtime_error("Failed to initialize encryption key.");
		}
	}
	else if (!loadKeyFromFile(keyFilePath))
	{
		throw std::runtime_error("Failed to load encryption key.");
	}

	std::random_device os_seed;
	const uint_least32_t seed = os_seed();
	this->seed = seed;

	level = new int[WIDTH * HEIGHT];

	try
	{
		// Attempt to generate the map using the seed
		MapGeneration::generate(this->seed, level, path);
	}
	catch (const std::exception& e)
	{
		// Handle any exceptions thrown by MapGeneration::generate
		std::cerr << "Error during map generation: " << e.what() << "\n";
		// Optionally, rethrow or handle the error in a way that makes sense for your program
		throw std::runtime_error("Failed to generate the map.\n");
	}

	// Create the ability tree
	abilityTree = AbilityTree::createAbilityTree();

	// Create the player
	player = new BoardGamePlayer();

}

Save::Save(const Save& save)
{
	level = new int[WIDTH * HEIGHT];
	std::copy_n(save.level, WIDTH * HEIGHT, level);

	this->key = save.key;
	this->slot = save.slot;
	this->seed = save.seed;
	this->path = save.path;
	this->abilityTree = save.abilityTree;

	this->player = save.player ? new BoardGamePlayer(*save.player) : new BoardGamePlayer();
}

Save& Save::operator=(const Save& save)
{
	if (this == &save) return *this;

	delete[] level;
	delete player;

	level = new int[WIDTH * HEIGHT];
	std::copy_n(save.level, WIDTH * HEIGHT, level);

	key = save.key;
	slot = save.slot;
	seed = save.seed;
	path = save.path;
	abilityTree = save.abilityTree;

	player = save.player ? new BoardGamePlayer(*save.player) : new BoardGamePlayer();

	return *this;
}

const std::string seedLine = "Seed: ";
const std::string levelLine = "Level: ";
const std::string pathLine = "Path: ";
const std::string abilitiesLine = "Abilities: ";
const std::string playerLine = "Player: ";

void Save::write(const int slot)
{
	this->slot = slot;

	Save::write(); // Call the default write method
}

void Save::write() const
{
	std::filesystem::path filepath = "src/saves/save" + std::to_string(slot) + ".sav";

	std::ostringstream oss;

	oss << seedLine << seed << "\n";
	oss << levelLine << serializeLevel() << "\n";
	oss << pathLine << serializePath() << "\n";
	oss << abilitiesLine << abilityTree->serialize() << "\n";
	oss << playerLine << player->serialize() << "\n";

	std::string plainText = oss.str();
	//std::cout << "Saved text:\n" << color("48E5C2", plainText) << "\n";
	std::vector<unsigned char> cipherText;
	std::vector<unsigned char> iv;

	if (key.empty())
	{
		std::cerr << "Error: Encryption key is not initialized.\n";
	}

	if (!encryptData(plainText, cipherText, iv))
	{
		std::cerr << "Encryption failed.\n";
		return;
	}

	std::ofstream file(filepath, std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "Failed to open save file for writing: " << filepath << "\n";
		return;
	}

	file.write(reinterpret_cast<const char*>(iv.data()), iv.size());
	file.write(reinterpret_cast<const char*>(cipherText.data()), cipherText.size());
	file.close();
}

Save& Save::load(int slot)
{
	this->slot = slot;

	return this->load();
}

Save& Save::load()
{
	std::cout << std::dec;
	std::filesystem::path filepath = "src/saves/save" + std::to_string(slot) + ".sav";
	std::ifstream file(filepath, std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "Save file not found: " << filepath << "\n";
		return *this;
	}

	// Read IV from the file
	std::vector<unsigned char> iv(IV_SIZE);
	file.read(reinterpret_cast<char*>(iv.data()), iv.size());

	// Read the rest of the file into cipherText
	std::vector<unsigned char> cipherText((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	// Decrypt the data
	std::string plainText;
	if (!decryptData(cipherText, iv, plainText))
	{
		std::cerr << "Decryption failed.\n";
		return *this;
	}
	std::cout << "Decrypted text:\n" << color("34E4EA", plainText) << "\n";
	// Process the decrypted plainText
	std::istringstream iss(plainText);
	std::string line;
	while (std::getline(iss, line))  // Process lines from plainText
	{
		if (line.starts_with(seedLine))
		{
			// Extract the seed after "Seed: "
			seed = static_cast<uint_least32_t>(std::stoul(line.substr(seedLine.size())));
		}
		else if (line.starts_with(levelLine))
		{
			level = deserializeLevel(line.substr(levelLine.size()));
		}
		else if (line.starts_with(pathLine))
		{
			path = deserializePath(line.substr(pathLine.size()));
		}
		else if (line.starts_with(abilitiesLine))
		{
			// Extract the abilities after "Abilities: "
			abilityTree = AbilityTree::deserialize(line.substr(abilitiesLine.size()));
		}
		else if (line.starts_with(playerLine))
		{
			// Extract the player after "Player: "
			player = BoardGamePlayer::deserialize(line.substr(playerLine.size()));
		}
	}

	return *this;
}