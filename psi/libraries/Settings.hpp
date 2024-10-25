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

inline void initializeSettings()
{//this function is meant to load database, general volume level etc.
	//img database for skills and cards generally
	//https://chatgpt.com/share/6718f803-c458-8002-b648-1b4b630eaf9a
}