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

	//img database for skills and cards generally
	//https://chatgpt.com/share/6718f803-c458-8002-b648-1b4b630eaf9a
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

	sqlite3_close(database);
	std::cout << "Closed SQLite database successfully\n";
}