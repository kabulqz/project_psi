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

	readabilityMode = false;
	//initializing volume settings from save - if not on save then standard
	//default values
	//values from 0 - 1
	general_audio = 0.1f;
	//values from 0 - 100
	ui_audio = 70.f;
	alert_audio = 70.f;
	ambience_audio = 70.f;
	music_audio = 70.f;

	const char* create_table_sql = R"(
		CREATE TABLE IF NOT EXISTS audio_settings (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			setting_name TEXT UNIQUE NOT NULL,
			setting_value REAL NOT NULL
		);
	)";
	char* error_message = nullptr;
	if (sqlite3_exec(database, create_table_sql, nullptr, nullptr, &error_message) != SQLITE_OK)
	{
		std::cerr << "B³¹d podczas tworzenia tabeli: " << error_message << std::endl;
		sqlite3_free(error_message);
		return;
	}
	std::cout << "Tabela audio_settings jest gotowa.\n";

	const char* sql = "SELECT setting_name, setting_value FROM audio_settings;";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(database, sql, -1, &stmt, nullptr) != SQLITE_OK)
	{
		std::cerr << "Failed to prepare SQL statement: " << sqlite3_errmsg(database) << std::endl;
		return;
	}

	const char* insert_defaults = R"(
	INSERT OR IGNORE INTO audio_settings (setting_name, setting_value)
	VALUES 
	('general_audio', 1.0),
	('ui_audio', 100.0),
	('alert_audio', 100.0),
	('ambience_audio', 100.0),
	('music_audio', 100.0);
)";

	sqlite3_exec(database, insert_defaults, nullptr, nullptr, &error_message);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		float value = static_cast<float>(sqlite3_column_double(stmt, 1));

		if (name == "general_audio")
			general_audio = value;
		else if (name == "ui_audio")
			ui_audio = value;
		else if (name == "alert_audio")
			alert_audio = value;
		else if (name == "ambience_audio")
			ambience_audio = value;
		else if (name == "music_audio")
			music_audio = value;
	}

	sqlite3_finalize(stmt);
	std::cout << "Loaded settings from the database.\n";
}

void Settings::saveSettings() const
{
	const char* sql = "REPLACE INTO audio_settings (setting_name, setting_value) VALUES (?, ?);";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(database, sql, -1, &stmt, nullptr) != SQLITE_OK)
	{
		std::cerr << "Failed to prepare SQL statement: " << sqlite3_errmsg(database) << std::endl;
		return;
	}

	auto saveSetting = [&](const std::string& name, float value) {
		sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_double(stmt, 2, value);
		if (sqlite3_step(stmt) != SQLITE_DONE)
		{
			std::cerr << "Error saving setting: " << name << " - " << sqlite3_errmsg(database) << std::endl;
		}
		else
		{
			// Log each saved setting
			std::cout << "Saved setting: " << name << " = " << value << "\n";
		}
		sqlite3_reset(stmt);
		};

	saveSetting("general_audio", general_audio);
	saveSetting("ui_audio", ui_audio);
	saveSetting("alert_audio", alert_audio);
	saveSetting("ambience_audio", ambience_audio);
	saveSetting("music_audio", music_audio);

	sqlite3_finalize(stmt);
	std::cout << "All settings saved to the database.\n";
}

void Settings::closeDB() const
{

	sqlite3_close(database);
	std::cout << "Closed SQLite database successfully\n";
}

std::string Settings::getFont() const
{
	switch (readabilityMode)
	{
	case false:
		return "src/img/normalFont.ttf";
	case true:
		return "src/img/readabilityFont.ttf";
	}
	return "";
}

int Settings::getFontSize() const
{
	switch (readabilityMode)
	{
	case false:
		return 21;
	case true:
		return 24;
	}
	return 20;
}
