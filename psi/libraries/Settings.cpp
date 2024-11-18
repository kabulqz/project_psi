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