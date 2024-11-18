#include "Save.hpp"

void Save::write(int slot) {

    std::string filename = "src/saves/save" + std::to_string(slot) + ".sav";

    std::ofstream file(filename);

    if (file.is_open()) {
        file << "Seed: " << seed << "\n";
        file.close();

    }
    else
    {
        std::cerr << "Failed to open save file for writing: " << filename << "\n";
    }
}

Save& Save::load(int slot) {
    std::string filename = "src/saves/save" + std::to_string(slot) + ".sav";
    std::ifstream file(filename);
    std::string fileseed;
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.rfind("Seed: ", 0) == 0) {
                fileseed = line.substr(6); // Extract the seed after "Seed: "
            }
        }
        file.close();
    }
    else {
        std::cerr << "Save file not found: " << filename << "\n";
    }
    return *this;
}

Save::Save() {
    using u32 = uint_least32_t;
    using engine = std::mt19937;
    std::random_device os_seed;
    const u32 seed = os_seed();
    engine generator(seed);
    this->seed = seed;
}