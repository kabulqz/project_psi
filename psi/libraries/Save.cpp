#include "Save.hpp"
#include "AbilityTree.hpp"

Save::Save()
{
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

	std::ofstream file(filepath);

	if (!file.is_open())
	{
		std::cerr << "Failed to open save file for writing: " << filepath << "\n";
	}
	file << "Seed: " << seed << "\n";
	file << "Abiliies: " << abilityTree->serialize() << "\n";

	file.close();
}

Save& Save::load(int slot)
{
	this->slot = slot;

	return this->load();
}

Save& Save::load()
{
	std::filesystem::path filepath = "src/saves/save" + std::to_string(slot) + ".sav";
	std::ifstream file(filepath);
	if (!file.is_open())
	{
		std::cerr << "Save file not found: " << filepath << "\n";
		return *this;
	}

	std::string line;
	while (std::getline(file, line))
	{
		if (line.starts_with("Seed: "))
		{
			// Extract the seed after "Seed: "
			seed = static_cast<uint_least32_t>(std::stoul(line.substr(6)));
		}
		else if (line.starts_with("Abilites: "))
		{
			// Extract the abilities after "Abilities: "
			abilityTree = AbilityTree::deserialize(line.substr(10));
		}
	}
	file.close();
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

	this->slot = save.slot;
	this->seed = save.seed;
	this->abilityTree = save.abilityTree;
	//this->mapGenerationType = save.mapGenerationType;
	//this->player = save.player;
	return *this;
}