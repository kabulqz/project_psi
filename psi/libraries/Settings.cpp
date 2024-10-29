#include "Settings.hpp"

//default values
//values from 0 - 1
float Settings::general_audio = 1;
//values from 0 - 100
float Settings::ui_audio = 20;


void Settings::initialize()
{//this function is meant to load database, general volume level etc.
	//img database for skills and cards generally
	//https://chatgpt.com/share/6718f803-c458-8002-b648-1b4b630eaf9a

	//initializing volume settings from save - if not on save then standard
}