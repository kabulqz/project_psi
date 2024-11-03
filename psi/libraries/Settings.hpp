#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <sqlite3.h>
#include <iostream>
#include <string.h>
#include <queue>
#include <memory>

//this class is defined here for Game States to manage it in .cpp files
//all functions from "State" family should be written in .cpp
class Game;

class Settings {
public:
	//values from 0 -1
	float general_audio;
	//values from 0 - 100
	float ui_audio;
	//database
	sqlite3* database;

	//this function is meant to load database, general volume level etc.
	void initialize();
};