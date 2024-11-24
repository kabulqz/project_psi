#pragma once

#include "Settings.hpp"

class Card;
class Effect;
class Hero;

enum class GameEvent
{
	TURN_START,         // Start of a turn
	TURN_END,           // End of a turn
	PLAYER_PLAY_CARD,   // Player plays a card
	PLAYER_DRAW_CARD,   // Player draws a card
	PLAYER_DISCARD_CARD,// Player discards a card
	UNIT_ATTACK,        // Unit attacks
	UNIT_DAMAGED,       // Unit is damaged
	UNIT_HEALED,        // Unit is healed
	UNIT_DEATH,         // Unit dies
	UNIT_SUMMONED,      // Unit is summoned
	UNIT_STATUS_APPLY,  // Unit has a status applied
	UNIT_STATUS_REMOVE, // Unit has a status removed
	UNIT_KEYWORD_ADD,   // Unit has a keyword added
	CARD_SHUFFLED,      // Card is shuffled into the deck
	CARD_STOLEN,        // Card is stolen

};

enum class Keyword
{
	INSTANT_ATTACK,     // Can attack immediately
	LIFE_STEAL,         // Heals the player when dealing damage
	DOUBLE_STRIKE,      // Can attack twice each turn
	DEFENDER,           // Must be attacked first
	IMMUNE,             // Cannot be targeted or affected by abilities while attacking
	POISONOUS,          // Kills any unit it damages
	BERSERK,            // Gains attack when damaged
	UNSTOPPABLE,        // Ignores defenders, being frozen and stunned
};

enum Status
{
	BLEEDING,           // Loses health at the start of the turn
	POISONED,           // Loses health at the end of the turn
	BURNING,            // Loses health when attacking
	FROZEN,             // Cannot attack
	STUNNED,            // Cannot act
	HEXED,              // Cannot use abilities
	MARKED,             // Takes extra damage
	CONFUSED,           // When attacking, target is random, even friendly
	ENRAGED,            // When damaged, gains attack
	CURSED,             // Cannot be healed or gain buffs while active
	SHIELDED            // Takes reduced damage for example maximum of 1 or 50%
};

enum class EffectTrigger
{
	IMMEDIATE,		// Triggered immediately upon activation
	ON_GAME_EVENT,	// Triggered when a specific GameEvent occurs
	ON_DRAW,		// Triggered when the card with this effect is drawn
	RECURRING		// Repeats based on intervals (e.g., every turn)
};

enum class EffectCategory
{
	BUFF,			// Buffs the card or unit				=> requires target, only valid for units or cards with mutable stats
	DEBUFF,			// Debuffs the card or unit				=> requires target, only valid for units or cards with mutable stats
	DAMAGE,			// Deals damage to the unit or player	=> requires numerical value, target must be valid for application (units, players)
	HEAL,			// Heals the unit or player				=> requires numerical value, target must be valid for application (units, players)
	//														   cannot exceed max health
	STATUS_APPLY,	// Applies a status to the unit			=> requires status identifier, target must support the applied status
	//														   define duration for the status (if temporary)
	STATUS_REMOVE,	// Removes a status from the unit		=> requires a list of removable statuses
	//														   target must have at least one applicable statuses
	KEYWORD_ADD,	// Adds a keyword to the unit			=> requires keyword identifier, target must support the keyword system
	//														   cannot apply the same keyword multiple times to the same unit
	SUMMON,			// Summons a unit						=> requires a card identifier or specifications for the summoned unit
	//														   must validate space validity in the target zone
	DRAW,			// Draws a card							=> requires a numerical value or a condition
	//														   must validate that the deck is not empty
	//														   specify who draws the card/s, optionally restrict to specific card types
	DISCARD,		// Discards a card						=> requires a numerical value or a condition
	//														   must validate that the player has enough cards in hand
	//														   specify who discards the card/s, optionally restrict to specific card types
	ENERGY_MODIFY,	// Modifies the energy of the player		=> requires a numerical value, must validate max or min energy limits
	//														   specify if the effect is temporary or permanent
	SILENCE,		// Silences the unit					=> requires target, only valid for units
	SHUFFLE,		// Shuffles the deck					=> requires card identifier or type
	//														   specify the target deck (player, opponent)
	STEAL			// Steals a card						=> requires target (player, opponent)
	//														   must define the source and destination zones
	//														   must validate that the target has cards in the source zone
};

enum class StatType
{
	ENERGY_COST,	// Energy cost - cards
	DAMAGE,			// Damage points - items
	DEFENSE,		// Defense points - items
	HEALTH,			// Health points - units
	ATTACK,			// Attack points - units
};

enum class TargetMode
{
	NONE,			// No specific target
	SELF,			// Targets the card/unit itself
	SINGLE,			// Targets a single other unit or player
	MULTIPLE,		// Targets multiple other units or/and player
	RANDOM_SINGLE,	// Targets a random single other unit or player
	RANDOM_MULTIPLE	// Targets a random multiple other units or/and player
};

enum class TargetGroup
{
	ALLY,			// Targets an ally
	ENEMY,			// Targets an enemy
	BOTH			// Targets both allies and enemies
};

enum class TargetZone
{
	NONE,			// No specific zone
	HAND,			// Targets cards in the hand
	DECK,			// Targets cards in the deck
	BATTLEFIELD		// Targets the units on the battlefield
};

enum class EffectDuration
{
	INSTANT, 		// Effect applies immediately and then is resolved (e.g. deal damage)
	PERMANENT,		// Effect lasts forever (e.g. buff stats)
	TURN_BASED,		// Effect lasts for a fixed number of turns (e.g. debuff stats)
	EVENT_BASED,	// Effect lasts until a specific event occurs (e.g. until the unit dies)
	OVER_TIME		// Effect applies over time (e.g. heal over time)
};

class Target
{
public:
	virtual ~Target() {}
};