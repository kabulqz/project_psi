#pragma once

#include "Settings.hpp"
#include "Card.hpp"
#include "Hero.hpp"

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
private:
	int slot;

	uint_least32_t  seed;
	std::shared_ptr<AbilityTree> abilityTree;
	//TypeOfMapGeneration mapGenerationType;
	//Hero player;
public:
	Save();//default constructor, so new save
	Save(const Save& save);
	~Save() = default;
	//Save(save data type from saves array in main menu state);
	uint_least32_t getSeed() { return seed; }
	void setSeed(uint_least32_t seed) { this->seed = seed; }
	std::shared_ptr<AbilityTree> getAbilityTree() { return abilityTree; }
	void setAbilityTree(std::shared_ptr<AbilityTree> abilityTree) { this->abilityTree = abilityTree; }

	Save& operator=(const Save& save);
	void write(int slot);	// write to specific slot
	void write() const;		// write to current slot
	Save& load(int slot);	// load from specific slot
	Save& load();			// load from current slot
	static std::optional<std::filesystem::file_time_type>  getLastWriteTime(int slot);
};