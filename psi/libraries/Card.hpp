#pragma once

#include "Settings.hpp"


enum class Card_type {
    Unit,
    Spell,
    Item
};


class Card {
    Settings settings;
    Card_type type;
    std::string background_img;    // Background image
    std::string cardface_1st_img;  // Card face (front)
    std::string cardface_2nd_img;  // Card face (unit/spell/item-specific)

    // Helper to convert string to Card_type
    Card_type stringToCardType(const std::string& typeStr) {
        if (typeStr == "Unit") return Card_type::Unit;
        if (typeStr == "Spell") return Card_type::Spell;
        if (typeStr == "Item") return Card_type::Item;
        throw std::invalid_argument("Unknown card type: " + typeStr);
    }

    void loadCards(Settings& settings) {
        sqlite3* db = settings.getDatabase(); // Get the database connection
        // Example: Load a card with ID 1
        Card card(1, db);
        card.print();
    }

    // Constructor to load card from database
    Card(int cardID, sqlite3*& Settings::database){
        std::string query = "SELECT type, background_img, cardface_1st_img, cardface_2nd_img FROM cards WHERE id = ?;";
     
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
            return;
        }

        // Bind the card ID
        sqlite3_bind_int(stmt, 1, cardID);

        // Execute the query
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // Fetch data from the row
            std::string typeStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            background_img = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            cardface_1st_img = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            const char* face2nd = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            cardface_2nd_img = face2nd ? face2nd : "";

            // Convert string to Card_type
            type = stringToCardType(typeStr);
        }
        else {
            std::cerr << "Card ID " << cardID << " not found in database.\n";
        }

        // Finalize the statement
        sqlite3_finalize(stmt);
    }

    // Print method for debugging
    void print() const {
        std::cout << "Card Details:\n"
            << "  Type: " << (type == Card_type::Unit ? "Unit" :
                type == Card_type::Spell ? "Spell" : "Item") << "\n"
            << "  Background Image: " << background_img << "\n"
            << "  First Card Face: " << cardface_1st_img << "\n"
            << "  Second Card Face: " << (cardface_2nd_img.empty() ? "None" : cardface_2nd_img) << "\n";
    }
};