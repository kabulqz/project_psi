#include "Settings.hpp"

//this function is meant to load database, general volume level etc.
void Settings::initialize()
{
	//database
	int exit = sqlite3_open("test.db", &database);

	if (exit)
	{
		std::cerr << "Error opening SQLite database\n" << sqlite3_errmsg(database);
		return;
	}
	std::cout << "Opened SQLite database successfully\n";


	//database .sav management
	//https://chatgpt.com/share/6739e40d-b30c-8002-a601-ddb60dfd0bb1

	//initializing volume settings from save - if not on save then standard

	//default values
	//values from 0 - 1
	general_audio = 0.5f;
	//values from 0 - 100
	ui_audio = 0.125f;
	alert_audio = 20.0f;
	environment_audio = 30.0f;
	music_audio = 50.0f;

}

void Settings::closeDB()
{
	sqlite3_close(database);
	std::cout << "Closed SQLite database successfully\n";
}

bool Settings::hasSaves() {
	sqlite3_stmt* stmt;
	const char* query = "SELECT COUNT(*) FROM saves";
	sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);
	bool result = false;
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		result = sqlite3_column_int(stmt, 0) > 0;
	}
	sqlite3_finalize(stmt);
	return result;
}

sqlite3* Settings::getDatabase() {
	return database;
}
