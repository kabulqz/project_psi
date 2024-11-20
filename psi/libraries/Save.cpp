#include "Save.hpp"

Save::Save()
{
	using u32 = uint_least32_t;
	using engine = std::mt19937;
	std::random_device os_seed;
	const u32 seed = os_seed();
	this->seed = seed;
}

void Save::write(int slot) const
{
	std::filesystem::path filepath = "src/saves/save" + std::to_string(slot) + ".sav";

	std::ofstream file(filepath);

	if (file.is_open()) 
	{
		file << "Seed: " << seed << "\n";
		file.close();
	}
	else
	{
		std::cerr << "Failed to open save file for writing: " << filepath << "\n";
	}
}

Save& Save::load(int slot)
{
	std::filesystem::path filepath = "src/saves/save" + std::to_string(slot) + ".sav";;
	std::ifstream file(filepath);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) 
		{
			if (line.starts_with("Seed: ")) 
			{
				std::string fileSeed = line.substr(6); // Extract the seed after "Seed: "
				try
				{
					seed = static_cast<uint_least32_t>(std::stoul(fileSeed));
				}
				catch (const std::invalid_argument& e) 
				{
					std::cerr << "Invalid seed value in save file: " << e.what() << "\n";
				}
				catch (const std::out_of_range& e) 
				{
					std::cerr << "Seed value out of range: " << e.what() << "\n";
				}
			}
		}
		file.close();
	}
	else 
	{
		std::cerr << "Save file not found: " << filepath << "\n";
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