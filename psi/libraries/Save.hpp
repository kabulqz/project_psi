#pragma once

#include "Settings.hpp"
#include "Game.hpp"
#include <iostream>
#include <sqlite3.h>

    class Save {
    private:
        Game* game;                // Pointer to a Game instance
        uint_least32_t saveSeed;   // Store the seed value
        Settings settings;

    public:
        Save(Game* gameInstance) : game(gameInstance), saveSeed(0) {
            if (game) {
                saveSeed = game->getSeed(); // Call getSeed() on the Game object
            }
        }

        void saveToDatabase1() {
            uint_least32_t gameSeed = game->getSeed();  // Access the seed from the Game object
            std::cout << "Saving game with seed: " << gameSeed << std::endl;
            // Use the seed in your database operations
        }

        uint_least32_t getSaveSeed() const { return saveSeed; }
        void setSaveSeed(uint_least32_t seed) { saveSeed = seed; }

        const char* createTableSQL = R"(
    CREATE TABLE IF NOT EXISTS saves (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        seed INTEGER NOT NULL,
        save_name TEXT NOT NULL
    );
)";

        void saveToDatabase(sqlite3* database) {
            if (!database) {
                std::cerr << "Database connection is null.\n";
                return;
            }

            const char* insertSQL = R"(
        INSERT INTO saves (seed, save_name)
        VALUES (?, ?);
    )";

            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(database, insertSQL, -1, &stmt, nullptr);

            if (rc != SQLITE_OK) {
                std::cerr << "Failed to prepare insert statement: " << sqlite3_errmsg(database) << "\n";
                return;
            }

            // Bind parameters
            sqlite3_bind_int(stmt, 1, saveSeed);              // Bind seed
            sqlite3_bind_text(stmt, 2, "Default Save", -1, SQLITE_STATIC); // Bind save_name

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                std::cerr << "Failed to execute insert statement: " << sqlite3_errmsg(database) << "\n";
            }
            else {
                std::cout << "Save inserted into database successfully.\n";
            }

            sqlite3_finalize(stmt); // Clean up
        }
};
