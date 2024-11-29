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

class Save {
private:
	int slot;

	uint_least32_t  seed;
	TypeOfMapGeneration mapGenerationType;

	Hero player;
	std::vector<Ability*> abilities;
public:
	Save();//default constructor, so new save
	Save(const Save& save);
	~Save() = default;
	//Save(save data type from saves array in main menu state);
	uint_least32_t getSeed() { return seed; }
	void setSeed(uint_least32_t seed)
	{
		this->seed = seed;
	}

	Save& operator=(const Save& save);
	void write(int slot);
	Save& load(int slot);
	Save& load();
	static std::optional<std::filesystem::file_time_type>  getLastWriteTime(int slot);
	std::vector<Ability*> getAbilities() { return abilities; }
	void setAbilities(const std::vector<Ability*>& abilities) { this->abilities = abilities; }

};