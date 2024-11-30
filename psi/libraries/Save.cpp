#include "Save.hpp"
#include "AbilityTree.hpp"

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
		/*
		std::cout << "Generated key: ";
		for (const auto& byte : key)
		{
			std::cout << std::hex << static_cast<int>(byte) << " ";
		}

		std::cout << std::dec << "\n";
		*/
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
	if (!EVP_EncryptUpdate(ctx, cipherText.data(), &len, reinterpret_cast<const unsigned char*>(plainText.data()), plainText.size()))
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
	if (!ctx) {
		std::cerr << "Failed to create cipher context.\n";
		return false;
	}

	if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()))
	{
		std::cerr << "Failed to initialize decryption.\n";
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	std::vector<unsigned char> plainTextBuf(cipherText.size() + EVP_MAX_BLOCK_LENGTH);
	int len = 0;
	if (!EVP_DecryptUpdate(ctx, plainTextBuf.data(), &len, cipherText.data(), cipherText.size()))
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
	if (!keyFile.is_open())
	{
		std::cerr << "Failed to open key for writing.\n";
		return false;
	}
	keyFile.write(reinterpret_cast<const char*>(key.data()), key.size());
	keyFile.close();
	return true;
}

bool Save::loadKeyFromFile(const std::string& keyFilePath)
{
	std::ifstream keyFile(keyFilePath, std::ios::binary);
	if (!keyFile.is_open())
	{
		std::cerr << "Failed to open key for reading.\n";
		return false;
	}

	key.resize(KEY_SIZE);  // Ensure this size matches the size used during saving
	keyFile.read(reinterpret_cast<char*>(key.data()), KEY_SIZE);
	if (keyFile.gcount() != KEY_SIZE)
	{
		std::cerr << "Invalid key file size.\n";
		return false;
	}
	keyFile.close();

	// Debugging: Check if key is correctly loaded
	std::cout << "Loaded key: ";
	for (unsigned char byte : key) {
		std::cout << std::hex << static_cast<int>(byte) << " ";
	}
	std::cout << std::endl;

	return true;
}

Save::Save()
{
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

	using u32 = uint_least32_t;
	using engine = std::mt19937;
	std::random_device os_seed;
	const u32 seed = os_seed();
	this->seed = seed;

	// Create abilities
	auto startAbility = std::make_shared<Ability>(0, 1, Ability::unlockStatus::UNLOCKED, 616, 605, sf::Vector2i(12, 2));
	auto heal1 = std::make_shared<Ability>(1, 1, Ability::unlockStatus::LOCKED, 617, 510, sf::Vector2i(9, 8));
	auto heal2 = std::make_shared<Ability>(4, 1, Ability::unlockStatus::LOCKED, 621, 403, sf::Vector2i(10, 8));
	auto heal3 = std::make_shared<Ability>(13, 1, Ability::unlockStatus::LOCKED, 620, 300, sf::Vector2i(11, 8));
	auto heal4 = std::make_shared<Ability>(17, 1, Ability::unlockStatus::LOCKED, 620, 188, sf::Vector2i(12, 8));
	auto fireball1 = std::make_shared<Ability>(5, 1, Ability::unlockStatus::LOCKED, 498, 392, sf::Vector2i(6, 0));
	auto fireball2 = std::make_shared<Ability>(15, 1, Ability::unlockStatus::LOCKED, 703, 244, sf::Vector2i(5, 0));
	auto freeze1 = std::make_shared<Ability>(7, 1, Ability::unlockStatus::LOCKED, 548, 337, sf::Vector2i(11, 6));
	auto freeze2 = std::make_shared<Ability>(12, 1, Ability::unlockStatus::LOCKED, 805, 250, sf::Vector2i(12, 6));
	auto plusSpeed1 = std::make_shared<Ability>(3, 1, Ability::unlockStatus::LOCKED, 694, 438, sf::Vector2i(14, 8));
	auto plusSpeed2 = std::make_shared<Ability>(8, 1, Ability::unlockStatus::LOCKED, 434, 345, sf::Vector2i(0, 9));
	auto buffMeleeUnits1 = std::make_shared<Ability>(2, 1, Ability::unlockStatus::LOCKED, 553, 443, sf::Vector2i(4, 9));
	auto buffMeleeUnits2 = std::make_shared<Ability>(11, 1, Ability::unlockStatus::LOCKED, 815, 347, sf::Vector2i(6, 9));
	auto buffMeleeUnits3 = std::make_shared<Ability>(19, 1, Ability::unlockStatus::LOCKED, 656, 101, sf::Vector2i(5, 9));
	auto buffRangedUnits = std::make_shared<Ability>(16, 1, Ability::unlockStatus::LOCKED, 774, 192, sf::Vector2i(8, 9));
	auto maxCardHand = std::make_shared<Ability>(9, 1, Ability::unlockStatus::LOCKED, 468, 269, sf::Vector2i(9, 9));
	auto maxHealth1 = std::make_shared<Ability>(10, 1, Ability::unlockStatus::LOCKED, 706, 331, sf::Vector2i(10, 9));
	auto maxHealth2 = std::make_shared<Ability>(14, 1, Ability::unlockStatus::LOCKED, 538, 218, sf::Vector2i(11, 9));
	auto maxEnergy1 = std::make_shared<Ability>(6, 1, Ability::unlockStatus::LOCKED, 755, 392, sf::Vector2i(3, 7));
	auto maxEnergy2 = std::make_shared<Ability>(18, 1, Ability::unlockStatus::LOCKED, 584, 101, sf::Vector2i(2, 7));

	// Setting up the tree
	abilityTree = std::make_shared<AbilityTree>(startAbility);
	startAbility->addChild(heal1);
	heal1->addChild(buffMeleeUnits1);
	heal1->addChild(plusSpeed1);
	heal1->addChild(heal2);
	heal2->addChild(fireball1);
	heal2->addChild(maxEnergy1);
	heal2->addChild(freeze1);
	freeze1->addChild(maxCardHand);
	freeze1->addChild(plusSpeed2);
	heal2->addChild(maxHealth1);
	maxHealth1->addChild(buffMeleeUnits2);
	maxHealth1->addChild(freeze2);
	heal2->addChild(heal3);
	heal3->addChild(maxHealth2);
	heal3->addChild(fireball2);
	fireball2->addChild(buffRangedUnits);
	heal3->addChild(heal4);
	heal4->addChild(buffMeleeUnits3);
	heal4->addChild(maxEnergy2);
}

Save::Save(const Save& save)
{
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

	this->slot = save.slot;
	this->seed = save.seed;
	this->abilityTree = save.abilityTree;
	//this->mapGenerationType = save.mapGenerationType;
	//this->player = save.player;
}

void Save::write(const int slot)
{
	this->slot = slot;

	Save::write(); // Call the default write method
}

void Save::write() const
{
	std::filesystem::path filepath = "src/saves/save" + std::to_string(slot) + ".sav";

	std::ostringstream oss;

	oss << "Seed: " << seed << "\n";
	oss << "Abiliies: " << abilityTree->serialize() << "\n";

	std::string plainText = oss.str();
	std::vector<unsigned char> cipherText;
	std::vector<unsigned char> iv;

	if (key.empty())
	{
		std::cerr << "Error: Encryption key is not initialized.\n";
	}
	/*
	std::cout << "Key size: " << key.size() << "\n"; 
	for (const auto& byte : key) {
		std::cout << std::hex << static_cast<int>(byte) << " ";
	}
	std::cout << std::dec << "\n";
	*/
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
	std::cout << "Decrypted text: " << plainText << "\n";
	// Process the decrypted plainText
	std::istringstream iss(plainText);
	std::string line;
	while (std::getline(iss, line))  // Process lines from plainText
	{
		if (line.starts_with("Seed: "))
		{
			// Extract the seed after "Seed: "
			seed = static_cast<uint_least32_t>(std::stoul(line.substr(6)));
			std::cout << "Seed: " << seed << "\n";
		}
		else if (line.starts_with("Abilities: "))
		{
			// Extract the abilities after "Abilities: "
			abilityTree = AbilityTree::deserialize(line.substr(10));
		}
	}

	return *this;
}

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

Save& Save::operator=(const Save& save)
{
	if (this == &save) return *this;

	this->key = save.key;
	this->slot = save.slot;
	this->seed = save.seed;
	this->abilityTree = save.abilityTree;
	//this->mapGenerationType = save.mapGenerationType;
	//this->player = save.player;
	return *this;
}