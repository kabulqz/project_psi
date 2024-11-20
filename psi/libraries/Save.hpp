#pragma once

#include "Settings.hpp"
#include "Card.hpp"

enum class TypeOfGeneration
{
	FUTURE,
	CLASS,
	FOR,
	TYPE,
	OF,
	MAP,
	GENERATION

};

class Save {
private:
	uint_least32_t  seed;
	TypeOfGeneration mapGenerationType;
	std::vector<Card> deck;
public:
	Save();//default constructor, so new save
	//Save(save data type from saves array in main menu state);
	uint_least32_t getSeed() { return seed; }
	void setSeed(uint_least32_t seed)
	{
		this->seed = seed;
	}

	void write(int slot) const;
	Save& load(int slot);
	static std::optional<std::filesystem::file_time_type>  getLastWriteTime(int slot);
};