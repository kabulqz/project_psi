#pragma once

#include "Settings.hpp"

class Card;
class Effect;
class Hero;

enum class GameEvent
{ // TODO: Add more events to track
	TURN_START,         // Start of a turn
	TURN_END,           // End of a turn
	CARD_PLAYED,        // Card has been played
	CARD_DRAWN,         // Card has been drawn
	CARD_DISCARDED,     // Card has been discarded
	CARD_SHUFFLED,      // Card has been shuffled into the deck
	CARD_STOLEN,        // Card has been stolen
	UNIT_ATTACK,        // Unit attacked
	UNIT_DAMAGED,       // Unit has been damaged
	UNIT_HEALED,        // Unit has been healed
	UNIT_DEATH,         // Unit has died
	UNIT_SUMMONED,      // Unit has been summoned
	UNIT_STATUS_APPLY,  // Unit has status applied
	UNIT_STATUS_REMOVE, // Unit has status removed
	UNIT_KEYWORD_ADD,   // Unit has keyword added
	SPELL_CASTED,       // Spell has been cast
	ITEM_EQUIPPED,      // Item has been equipped
};

enum class Keyword // In alphabetical order
{
	BERSERK,        // Gains attack after it is attacked
	BLOODTHIRSTY,   // Gains attack whenever enemy unit has bleeding status
	DEFENDER,       // Must be attacked first
	DOUBLE_STRIKE,  // Can attack twice each turn
	ICEBREAKER,		// Gains health whenever it kills a frozen unit
	IMMUNE,         // Cannot be chosen as a target for effects
	INSTANT_ATTACK, // Can attack immediately
	INSTANT_KILLER, // Kills any unit it damages
	LIFE_STEAL,     // Heals the player when dealing damage
	MASTER_OF_ARMS, // Gains attack whenever it is equipped with an item
	PYROMANIAC,     // Gains attack whenever enemy unit has burning status
	RANGED,         // When attacking, doesn't take damage back
	UNSTOPPABLE		// Ignores defenders, being frozen and stunned
};

enum Status // In alphabetical order
{
	BLEEDING,       // Loses health at the start of the turn
	BURNING,        // Loses health when attacking
	CONFUSED,       // When attacking, target is random, even friendly
	CURSED,         // Cannot be healed or gain buffs while active
	DAMAGED,        //		=> Status to track damage over time, no other interactions
	ENRAGED,        // When damaged, gains attack
	FROZEN,         // Cannot attack
	HEALING,        //		=> Status to track healing over time, no other interactions
	HEXED,          // Cannot use effects from this unit
	MARKED,         // Takes extra damage
	POISONED,       // Loses health at the end of the turn
	ROCK_SKINNED,   // Takes reduced damage for example maximum of 1 or 50%
	SILENCED,       //		=> Status to track silencing, no other interactions
	STUNNED			// Cannot add items to this unit, if unit has already items, they are removed
};

enum class EffectCategory
{
	BUFF,			// Buffs the card or unit				=> requires target, only valid for units or cards with mutable stats
	DEBUFF,			// Debuffs the card or unit				=> requires target, only valid for units or cards with mutable stats
	HEAL,			// Heals the unit or player				=> requires numerical value, target must be valid for application (units, players)
	//														   cannot exceed max health
	DAMAGE,			// Deals damage to the unit or player	=> requires numerical value, target must be valid for application (units, players)
	STATUS_APPLY,	// Applies a status to the unit			=> requires status identifier, target must support the applied status
	//														   define duration for the status (if temporary)
	STATUS_REMOVE,	// Removes a status from the unit		=> requires a list of removable statuses
	//														   target must have at least one applicable statuses
	KEYWORD_ADD,	// Adds a keyword to the unit			=> requires keyword identifier, target must support the keyword system
	//														   cannot apply the same keyword multiple times to the same unit
	SUMMON,			// Summons a unit						=> requires a card identifier or specifications for the summoned unit
	//														   must validate space validity in the target zone
	CAST,			// Casts a spell						=> requires a card identifier or specifications for the casted spell
	EQUIP,			// Equips an item						=> requires a card identifier or specifications for the equipped item,
	//														   must validate that the target can equip the item
	DRAW,			// Draws a card							=> requires a numerical value or a condition
	//														   must validate that the deck is not empty
	//														   specify who draws the card/s, optionally restrict to specific card types
	DISCARD,		// Discards a card						=> requires a numerical value or a condition
	//														   must validate that the player has enough cards in hand
	//														   specify who discards the card/s, optionally restrict to specific card types
	ENERGY_MODIFY,	// Modifies the energy of the player	=> requires a numerical value, must validate max or min energy limits
	//														   specify if the effect is temporary or permanent
	SILENCE,		// Silences the unit					=> requires target, only valid for units
	SHUFFLE,		// Shuffles the deck					=> requires card identifier or type
	//														   specify the target deck (player, opponent)
	STEAL			// Steals a card						=> requires target (player, opponent)
	//														   must define the source and destination zones
	//														   must validate that the target has cards in the source zone
};

enum class StatType // Type of stat to buff/debuff
{
	ENERGY_COST,	// Energy cost - cards
	DAMAGE,			// Damage points - items
	DEFENSE,		// Defense points - items
	HEALTH,			// Health points - units
	ATTACK,			// Attack points - units
};

enum class EffectTrigger
{
	IMMEDIATE,		// Triggered immediately when the card is played
	ON_GAME_EVENT,	// Triggered when a specific GameEvent occurs
	ON_DRAW,		// Triggered when the card with this effect is drawn		=> check for this effect when drawing a card
	ON_DISCARD,		// Triggered when the card with this effect is discarded	=> check for this effect when discarding a card
	ON_ATTACK,		// Triggered when the unit with this effect attacks			=> check for this effect when unit is attacking
	ON_EQUIP,		// Triggered when the item with this effect is equipped		=> check for this effect when equipping an item
};

enum class EffectDuration
{
	INSTANT, 		// Effect applies immediately and then is resolved (e.g. deal damage)
	PERMANENT,		// Effect lasts forever (e.g. buff stats)
	TURN_BASED,		// Effect lasts for a fixed number of turns (e.g. debuff stats)
	EVENT_BASED,	// Effect lasts until a specific event occurs (e.g. until the unit dies)
	OVER_TIME		// Effect applies over time (e.g. heal over time)
};

enum class TargetMode
{
	NONE,			// No specific target, so a global effect
	SELF,			// Targets the card/unit itself
	SINGLE,			// Targets a single other unit or player
	MULTIPLE,		// Targets multiple other units or/and player
	RANDOM_SINGLE,	// Targets a random single other unit or player
	RANDOM_MULTIPLE	// Targets a random multiple other units or/and player
};

enum class TargetZone
{
	NONE,			// No specific zone
	HAND,			// Targets cards in the hand
	DECK,			// Targets cards in the deck
	BATTLEFIELD		// Targets the units on the battlefield
};

enum class TargetGroup
{
	ALLY,			// Targets an ally
	ENEMY,			// Targets an enemy
	BOTH			// Targets both allies and enemies
};

class Target
{
public:
	virtual ~Target() {}
};