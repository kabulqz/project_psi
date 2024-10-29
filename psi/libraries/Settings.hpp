#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <sqlite3.h>
#include <iostream>
#include <string.h>
#include <memory>

//this class is defined here for Game States to menage it in .cpp files
//all functions from "State" family should be written in .cpp
class Game;

class Settings {
public:
	//values from 0 -1
	static float general_audio;
	//values from 0 - 100
	static float ui_audio;
	//database
	sqlite3* database;

	static void initialize();
};