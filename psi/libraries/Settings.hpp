#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <windows.h>
#include <sqlite3.h>

#include <unordered_set>
#include <functional>
#include <filesystem>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <memory>
#include <cctype>
#include <random>
#include <thread>
#include <queue>
#include <array>
#include <map>

inline std::string color(const std::string& hex_color, const std::string& data)
{
	int r = std::stoi(hex_color.substr(0, 2), nullptr, 16);
	int g = std::stoi(hex_color.substr(2, 2), nullptr, 16);
	int b = std::stoi(hex_color.substr(4, 2), nullptr, 16);
	std::string out = "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m" + data
		+ "\033[0m";
	return out;
}

//this class is defined here for Game States to manage it in .cpp files
//all functions from "State" family should be written in .cpp
class Game;

class Settings {
public:
	//values from 0 -1
	float general_audio;
	//values from 0 - 100
	float ui_audio;
	float alert_audio;
	float environment_audio;
	float music_audio;
	//database
	sqlite3* database;
	//this function is meant to load database, general volume level etc.
	void initialize();
	void closeDB();
};
