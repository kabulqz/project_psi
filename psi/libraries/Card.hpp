#pragma once

#include "Settings.hpp"

enum class Card_type
{
	Unit,
	Spell,
	Item
};

class Card
{
private: //getting img elements from database 
	Card_type type;
	std::string background_img;		//background image
	std::string cardface_1st_img;	//cardface front
	std::string cardface_2nd_img;	//cardface unit / spell / item
public:
	//constructor which uses database to load img elements to build cards
	//print() method to print card in hand
};