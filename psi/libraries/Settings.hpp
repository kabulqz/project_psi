#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <windows.h>
#include <sqlite3.h>

#include <unordered_set>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <memory>
#include <cctype>
#include <random>
#include <thread>
#include <future>
#include <vector>
#include <stack>
#include <queue>
#include <array>
#include <map>

constexpr int WIDTH = 60;
constexpr int HEIGHT = 60;

const std::string PATH_TO_BORDERS_FOLDER = "src/img/borders/";
const std::string PATH_TO_ABILITIES_FOLDER = "src/img/";

const std::string font = "src/img/antiquity-print.ttf";
//const std::string font = "src/img/easvhs.ttf";
constexpr int fontSize = 21;
//constexpr int fontSize = 24;

inline std::string color(const std::string& hex_color, const std::string& data)
{
	int r = std::stoi(hex_color.substr(0, 2), nullptr, 16);
	int g = std::stoi(hex_color.substr(2, 2), nullptr, 16);
	int b = std::stoi(hex_color.substr(4, 2), nullptr, 16);
	std::string out = "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m" + data
		+ "\033[0m";
	return out;
}

inline std::string gradient(const std::string& start_hex_color, const std::string& end_hex_color, const std::string& text)
{
	int r1 = std::stoi(start_hex_color.substr(0, 2), nullptr, 16);
	int g1 = std::stoi(start_hex_color.substr(2, 2), nullptr, 16);
	int b1 = std::stoi(start_hex_color.substr(4, 2), nullptr, 16);

	int r2 = std::stoi(end_hex_color.substr(0, 2), nullptr, 16);
	int g2 = std::stoi(end_hex_color.substr(2, 2), nullptr, 16);
	int b2 = std::stoi(end_hex_color.substr(4, 2), nullptr, 16);

	// Generate gradient
	size_t length = text.size();
	std::string result;
	for (size_t i = 0; i < length; ++i)
	{
		// Interpolate RGB values
		float t = static_cast<float>(i) / static_cast<float>(length - 1);
		int r = static_cast<int>(r1 + t * (r2 - r1));
		int g = static_cast<int>(g1 + t * (g2 - g1));
		int b = static_cast<int>(b1 + t * (b2 - b1));

		// Convert RBG back to hex
		char hexColor[7];
		snprintf(hexColor, sizeof(hexColor), "%02X%02X%02X", r, g, b);

		// Apply color to the current character
		result += color(hexColor, std::string(1, text[i]));
	}
	return result;
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
	float ambience_audio;
	float music_audio;
	//database
	sqlite3* database;
	//this function is meant to load database, general volume level etc.
	void initialize();
	void closeDB() const;
};
